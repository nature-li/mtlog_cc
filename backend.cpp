// backend.cpp

#include <iostream>
#include <errno.h>
#include <string.h>
#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>
#include "backend.h"
#include "tool.h"

namespace mtad {
namespace adlog {

Backend::Backend(std::string dir, long max_size, int file_count,
                 const std::map<int, std::string>& file_names)
    : dir_(dir),
      max_size_(max_size),
      file_count_(file_count),
      flag_(0) {
  // Create FileInfo for every level
  for (auto it = file_names.begin(); it != file_names.end(); it++) {
    // file level and file name
    const std::string& name = it->second;
    std::string full_name = dir_ + "/" + name;

    // open file if necessary
    std::shared_ptr<FileInfo> file;
    auto finder = name_map_.find(full_name);
    if (finder == name_map_.end()) {
      file = std::make_shared<FileInfo>();
      file->SetFileName(full_name);
      name_map_[full_name] = file;
    } else {
      file = finder->second;
    }

    // store level to file mapping
    file_info_.push_back(file);
  }
}

Backend::~Backend() {
  // Close all file
  Stop();
}

bool Backend::Init() {
  // Create dir if necessary
  DIR* dp = opendir(dir_.c_str());
  if (dp) {
    closedir(dp);
  } else {
    int ret = mkdir(dir_.c_str(), 0755);
    if (ret != 0) {
      std::ostringstream oss;
      oss << __FILE__ << ":" << __LINE__ << " mkdir " << dir_ << "error: "
          << strerror(errno) << std::endl;
      sys_log_.LogError(oss.str().c_str());
      return false;
    }
  }

  // open files
  for (auto it = name_map_.begin(); it != name_map_.end(); it++) {
    FileInfo* file = it->second.get();
    file->SetSysLog(&sys_log_);
    if (!file->Open()) {
      return false;
    }
  }

  // start clean thread
  if (file_count_ >= 0) {
    thread_ = std::thread(&Backend::DeleteExpireLog, this);
  }
  return true;
}

void Backend::Stop() {
  // set thread quit flag
  {
    std::unique_lock<std::mutex> lock(mutex_);
    flag_ = 1;
    cond_.notify_one();
  }

  // flush and close all files
  for (auto it = name_map_.begin(); it != name_map_.end(); it++) {
    std::shared_ptr<FileInfo> file = it->second;
    if (file) {
      int fd = file->GetFd();
      if (fd != -1) {
        int ret = close(fd);
        if (ret != 0) {
          std::ostringstream oss;
          oss << __FILE__ << ":" << __LINE__ << " close error: "
              << strerror(errno) << std::endl;
          sys_log_.LogError(oss.str().c_str());
        }
      }
    }
  }
  name_map_.clear();
  file_info_.clear();

  // wait thread to quit
  if (file_count_ >= 0) {
    thread_.join();
  }
}

bool Backend::WriteFlushRotate(Record* record) {
  FileInfo* file = file_info_[record->level_].get();
  file->Put(record->content_, record->content_used_);
  file->Flush();
  if (file->NeedRotate(max_size_)) {
    file->Rotate();
  }
  return true;
}

bool Backend::Write(QueuePtr queue_ptr) {
  // Write all log
  int size = queue_ptr->Size();
  std::set<FileInfo*> a_set;

  // Flush every 1024 data to disk
  for (int i = 0; i < size; i++) {
    Record* record = queue_ptr->Get(i);
    FileInfo* file = file_info_[record->level_].get();
    a_set.insert(file);

    if (record->content_used_ > 0) {
      file->Put(record->content_, record->content_used_);
      if (file->Full()) {
        file->Flush();
      }
    }
  }

  // Flush left data to disk
  for (std::set<FileInfo*>::iterator it = a_set.begin(); it != a_set.end();
      it++) {
    FileInfo* file = *it;
    file->Flush();
  }

  // Delete data
  while (!queue_ptr->Empty()) {
    Record* record = queue_ptr->Pop();
    delete record;
  }

  return true;
}

bool Backend::Rotate() {
  for (auto it = name_map_.begin(); it != name_map_.end(); it++) {
    FileInfo* file = it->second.get();

    if (file->NeedRotate(max_size_)) {
      file->Rotate();
    }

    // size rotate
    bool size_rotate = false;
    long file_size = file->FileSize();
    if (file_size > max_size_) {
      size_rotate = true;
    }
  }

  return true;
}

void Backend::DeleteExpireLog() {
  typedef std::map<std::string, std::string> Time2Name;
  typedef std::shared_ptr<Time2Name> Time2NamePtr;
  typedef std::map<std::string, Time2NamePtr> PrefixDict;

  // do NOT need to delete expired files
  if (file_count_ < 0) {
    return;
  }

  while (1) {
    // open directory
    DIR* dr = opendir(dir_.c_str());
    if (!dr) {
      continue;
    }

    // list all file in the directory
    PrefixDict prefix_dict;
    while (true) {
      struct dirent* drp = readdir(dr);
      if (!drp) {
        break;
      }

      // Example:
      // file_name is server.process.log.20161026145203123456
      // prefix is server.process.log
      // tail is 20161026145203123456
      std::string file_name = drp->d_name;
      std::string prefix;
      std::string tail;
      std::size_t idx = file_name.rfind('.');
      if (idx != std::string::npos) {
        prefix = file_name.substr(0, idx);
        tail = file_name.substr(idx + 1);
      }

      // check whether front is valid
      std::string raw_name = dir_ + "/" + prefix;
      auto find = name_map_.find(raw_name);
      if (find == name_map_.end()) {
        continue;
      }

      // length(20161026143302123456) == 20
      if (tail.length() != 20) {
        continue;
      }
      std::string sec_str = tail.substr(0, 14);

      // get time_t from log file name
      time_t when;
      bool ret = Tool::GetTimeFromString(sec_str, when);
      if (!ret) {
        continue;
      }

      Time2NamePtr time_2_name_ptr;
      auto it = prefix_dict.find(prefix);
      if (it == prefix_dict.end()) {
        time_2_name_ptr = std::make_shared<Time2Name>();
        prefix_dict[prefix] = time_2_name_ptr;
      } else {
        time_2_name_ptr = it->second;
      }

      time_2_name_ptr->insert(std::make_pair(tail, file_name));
    }

    // close directory
    closedir(dr);

    // delete old files for every prefix set
    for (auto it = prefix_dict.begin(); it != prefix_dict.end(); it++) {
      Time2NamePtr time_2_name_ptr = it->second;

      // check file count
      int file_count = time_2_name_ptr->size();
      if (file_count <= file_count_) {
        continue;
      }

      for (auto iter = time_2_name_ptr->begin(); iter != time_2_name_ptr->end();
          iter++) {
        if (file_count > file_count_) {
          std::string full_path = dir_ + "/" + iter->second;
          remove(full_path.c_str());
          file_count--;
        } else {
          break;
        }
      }
    }

    // every 60 seconds make a check
    {
      std::unique_lock<std::mutex> lock(mutex_);
      if (flag_ != 1) {
        cond_.wait_for(lock, std::chrono::seconds(60));
      }

      if (flag_ == 1) {
        break;
      }
    }
  }
}

}
}
