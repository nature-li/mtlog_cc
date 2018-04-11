// sys_log.cpp

#include "sys_log.h"

namespace mtad {
namespace adlog {

SysLog::SysLog()
  : seconds_interval_(3600)
  , max_per_interval_(10) {
  setlogmask(LOG_UPTO(LOG_NOTICE));
  openlog("mt_log", LOG_CONS | LOG_PID | LOG_NDELAY | LOG_PERROR, LOG_USER);

  last_time_ = 0;
  count_per_interval_ = 0;
}

SysLog::~SysLog() {
  closelog();
}

void SysLog::LogError(const char* msg) {
  if (ShouldLog()) {
    syslog(LOG_ERR, msg);
  }
}

bool SysLog::ShouldLog() {
  time_t now = time(NULL);

  // one hour later reset counter
  if (now - last_time_ >= seconds_interval_) {
    last_time_ = now;
    count_per_interval_ = 0;
  }

  // max 10 logs per hour
  if (count_per_interval_ >= max_per_interval_) {
    return false;
  }

  count_per_interval_++;
  return true;
}

}
}

