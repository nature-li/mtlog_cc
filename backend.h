// backend.h

#ifndef BACKEND_H_
#define BACKEND_H_

#include <set>
#include <sys/time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/uio.h>
#include <limits.h>
#include <queue>
#include <memory>
#include <map>
#include <thread>
#include <condition_variable>
#include "record.h"
#include "queue.h"
#include "fileinfo.h"
#include "sys_log.h"

namespace mtad {
namespace adlog {

class Backend {
  typedef Queue<Record*> LogQueue;
  typedef LogQueue* QueuePtr;
 public:
  Backend(std::string dir, long max_size, int file_count,
          const std::map<int, std::string>& file_names);

  ~Backend();

  bool Init();

  void Stop();

  bool WriteFlushRotate(Record* record);

  bool Write(QueuePtr queue_ptr);

  bool Rotate();

 private:
  void DeleteExpireLog();

 private:
  std::string dir_;
  long max_size_;
  int file_count_;
  std::thread thread_;
  int flag_;
  std::condition_variable cond_;
  std::mutex mutex_;

  std::map<std::string, std::shared_ptr<FileInfo>> name_map_;
  std::vector<std::shared_ptr<FileInfo>> file_info_;

  SysLog sys_log_;
};

}
}

#endif /* BACKEND_H_ */
