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

FileInfo::FileInfo()
    : fd_(-1),
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

int FileInfo::Flush() {
  int ret = 0;
  if (count_ > 0) {
    ret = writev(fd_, vec_, count_);
    if (ret == -1) {
      ret_code_ = ret;
      std::ostringstream oss;
      oss << __FILE__ << ":" << __LINE__ << " writev error: " << strerror(errno)
          << std::endl;
      sys_log_->LogError(oss.str().c_str());
    }
    count_ = 0;
  }

  return ret;
}

void FileInfo::SetFd(int fd) {
  fd_ = fd;
}

int FileInfo::GetFd() {
  return fd_;
}

void FileInfo::SetFileName(const std::string& file_name) {
  file_name_ = file_name;
}

std::string FileInfo::GetFileName() {
  return file_name_;
}

long FileInfo::FileSize() {
  long size = lseek(fd_, 0, SEEK_END);
  if (size == -1) {
    ret_code_ = size;
  }

  return size;
}

struct tm FileInfo::GetLastRenameTime() {
  return last_name_time_;
}

int FileInfo::GetRetValue() {
  return ret_code_;
}

void FileInfo::SetSysLog(SysLog* sys_log) {
  sys_log_ = sys_log;
}

void FileInfo::Reset() {
  ret_code_ = 0;
  fd_ = -1;
  count_ = 0;
  ret_code_ = 0;
}

void FileInfo::Reset(int fd, struct tm& last_name_time) {
  Reset();

  fd_ = fd;
  last_name_time_ = last_name_time;
}

bool FileInfo::NeedRotate(long max_size) {
  struct tm now;
  Tool::GetCurTime(now);

  // size rotate
  long file_size = this->FileSize();
  if (file_size > max_size) {
    return true;
  }

  // zero night rotate
  struct tm last_name_time = this->GetLastRenameTime();
  if (now.tm_year != last_name_time.tm_year
      || now.tm_mon != last_name_time.tm_mon
      || now.tm_mday != last_name_time.tm_mday) {
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
  // close file
  this->Close();

  // rename or delete file
  long size = lseek(fd_, 0, SEEK_END);
  if (size > 0) {
    this->Rename();
  } else {
    this->Delete();
  }

  // reopen file
  this->Open();
}

}
}
