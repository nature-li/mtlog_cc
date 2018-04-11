// sys_log.h

#ifndef MTLOG_SYS_LOG_H_
#define MTLOG_SYS_LOG_H_

#include <syslog.h>
#include <time.h>

namespace mtad {
namespace adlog {

class SysLog {
 public:
  SysLog();

  ~SysLog();

  void LogError(const char* msg);

 private:
  bool ShouldLog();

 private:
  time_t last_time_;

  // max 10 logs per hour
  const long seconds_interval_;
  const int max_per_interval_;
  int count_per_interval_;
};

}
}

#endif /* MTLOG_SYS_LOG_H_ */
