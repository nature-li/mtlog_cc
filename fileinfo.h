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
  FileInfo(long max_length);

  ~FileInfo();

  bool Full();

  void Put(void* base, size_t len);

  void WriteAndFlush(void* base, size_t len);

  int FlushAndRotate();

  void SetFileName(const std::string& file_name);

  void SetSysLog(SysLog* sys_log);

  void Reset();

  void Reset(int fd, struct tm& last_name_time);

  bool NeedRotate();

  bool Open();

  bool Close();

  bool Rename();

  void Delete();

  void Rotate();

 private:
  int fd_;
  long cur_length_;
  long max_length_;
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
