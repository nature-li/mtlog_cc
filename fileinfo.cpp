// fileinfo.cpp

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sstream>
#include "fileinfo.h"
#include "tool.h"

namespace mtad {
namespace adlog {

FileInfo::FileInfo(long max_length)
    : fd_(-1),
      cur_length_(0),
      max_length_(max_length),
      count_(0),
      ret_code_(0),
      sys_log_(NULL) {
}

FileInfo::~FileInfo() {
}

bool FileInfo::Full() {
  return count_ == IOV_MAX;
}

void FileInfo::Put(void* base, size_t len) {
  vec_[count_].iov_base = base;
  vec_[count_].iov_len = len;
  count_++;
}

int FileInfo::FlushAndRotate() {
  int ret = 0;
  if (count_ > 0) {
    ret = writev(fd_, vec_, count_);
    count_ = 0;

    if (ret == -1) {
      ret_code_ = ret;
      std::ostringstream oss;
      oss << __FILE__ << ":" << __LINE__ << " writev error: " << strerror(errno)
          << std::endl;
      sys_log_->LogError(oss.str().c_str());
      return ret;
    }

    cur_length_ += ret;
    if (cur_length_ >= max_length_) {
      this->Rotate();
    }
  }

  return ret;
}

void FileInfo::SetFileName(const std::string& file_name) {
  file_name_ = file_name;
}

void FileInfo::SetSysLog(SysLog* sys_log) {
  sys_log_ = sys_log;
}

void FileInfo::Reset() {
  ret_code_ = 0;
  fd_ = -1;
  cur_length_ = 0;
  count_ = 0;
  ret_code_ = 0;
}

void FileInfo::Reset(int fd, struct tm& last_name_time) {
  Reset();

  fd_ = fd;
  cur_length_ = lseek(fd_, 0, SEEK_END);
  last_name_time_ = last_name_time;
}

bool FileInfo::NeedRotate() {
  struct tm now;
  Tool::GetCurTime(now);

  // size rotate
  if (cur_length_ > max_length_) {
    return true;
  }

  // zero night rotate
  if (now.tm_year != last_name_time_.tm_year
      || now.tm_mon != last_name_time_.tm_mon
      || now.tm_mday != last_name_time_.tm_mday) {
    return true;
  }

  // error rotate
  if (ret_code_ != 0) {
    return true;
  }

  return false;
}

bool FileInfo::Open() {
  // open file
  int fd = open(file_name_.c_str(), O_CREAT | O_APPEND | O_RDWR, 0666);
  if (fd == -1) {
    std::ostringstream oss;
    oss << __FILE__ << ":" << __LINE__ << " open " << file_name_ << " error: "
        << strerror(errno) << std::endl;
    sys_log_->LogError(oss.str().c_str());
    return false;
  }

  // get file modify time
  struct stat attr;
  stat(file_name_.c_str(), &attr);
  time_t zone_now = attr.st_mtime + 28800;

  // time_t to time structure
  struct tm last_name_time;
  gmtime_r(&zone_now, &last_name_time);

  // reset
  this->Reset(fd, last_name_time);
  return true;
}

bool FileInfo::Close() {
  int ret = close(fd_);
  if (ret != 0) {
    std::ostringstream oss;
    oss << __FILE__ << ":" << __LINE__ << " close " << file_name_ << " error: "
        << strerror(errno) << std::endl;
    sys_log_->LogError(oss.str().c_str());
  }

  this->Reset();
  return true;
}

bool FileInfo::Rename() {
  std::string now = Tool::GetTimeMicroString(false);
  std::string new_name = file_name_ + "." + now;
  int ret = rename(file_name_.c_str(), new_name.c_str());
  if (ret != 0) {
    std::ostringstream oss;
    oss << __FILE__ << ":" << __LINE__ << " rename " << file_name_ << " error: "
        << strerror(errno) << std::endl;
    sys_log_->LogError(oss.str().c_str());
  }
  return true;
}

void FileInfo::Delete() {
  int ret = remove(file_name_.c_str());
  if (ret != 0) {
    std::ostringstream oss;
    oss << __FILE__ << ":" << __LINE__ << " remove " << file_name_ << " error: "
        << strerror(errno) << std::endl;
    sys_log_->LogError(oss.str().c_str());
  }
}

void FileInfo::Rotate() {
  // rename or delete file
  if (cur_length_ > 0) {
    this->Close();
    this->Rename();
  } else {
    this->Close();
    this->Delete();
  }

  // reopen file
  this->Open();
}

void FileInfo::WriteAndFlush(void* base, size_t len) {
  size_t count = write(fd_, base, len);
  if (count < len) {
    std::ostringstream oss;
    oss << __FILE__ << ":" << __LINE__ << " write " << file_name_ << " size:"
        << count << " < len:" << len << std::endl;
    sys_log_->LogError(oss.str().c_str());
  }
}

}
}
