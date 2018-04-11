// fileinfo.h

#ifndef FILE_INFO_
#define FILE_INFO_

#include <iostream>
#include <sys/uio.h>
#include <limits.h>
#include "sys_log.h"


namespace mtad {
namespace adlog {

class FileInfo {
 public:
  FileInfo();

  ~FileInfo();

  bool Full();

  void Put(void* base, size_t len);

  int Flush();

  void SetFd(int fd);

  int GetFd();

  void SetFileName(const std::string& file_name);

  long FileSize();

  std::string GetFileName();

  struct tm GetLastRenameTime();

  int GetRetValue();

  void SetSysLog(SysLog* sys_log);

  void Reset();

  void Reset(int fd, struct tm& last_name_time);

  bool NeedRotate(long max_size);

  bool Open();

  bool Close();

  bool Rename();

  void Delete();

  void Rotate();

 private:
  int fd_;
  std::string file_name_;
  struct tm last_name_time_;
  int count_;
  struct iovec vec_[IOV_MAX];
  int ret_code_;
  SysLog* sys_log_;
};

}
}
#endif
