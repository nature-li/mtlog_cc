// tool.cpp

#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "tool.h"

namespace mtad {
namespace adlog {

std::string Tool::log_level_[] = { "trace", "debug", "info", "warn", "error",
    "fatal", "report", "invalid_sev" };
std::string Tool::log_env_[] = { "product", "abtest", "develop", "invalid_env" };

thread_local TmTime Tool::tm_time_;
thread_local DigitTime Tool::digit_time_;
thread_local AlphaTime Tool::alpha_time_;
thread_local std::string Tool::thread_id_;

Tool::Tool() {
  // TODO Auto-generated constructor stub

}

Tool::~Tool() {
  // TODO Auto-generated destructor stub
}

void Tool::GetCurTime(struct tm& base) {
  time_t now = time(NULL);
  if (now != Tool::tm_time_.sec_) {
    time_t zone_now = now + 28800;
    gmtime_r(&zone_now, &base);

    Tool::tm_time_.sec_ = now;
    Tool::tm_time_.tm_ = base;
    return;
  }

  base = Tool::tm_time_.tm_;
  return;
}

bool Tool::GetTimeFromString(const std::string& str, time_t& when) {
  struct tm tm;
  sscanf(str.c_str(), "%04d%02d%02d%02d%02d%02d", &tm.tm_year, &tm.tm_mon,
         &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec);

  // check time
  char check[32] = { 0 };
  snprintf(check, 32, "%04d%02d%02d%02d%02d%02d", tm.tm_year, tm.tm_mon,
           tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  if (str != check) {
    return false;
  }

  when = mktime(&tm);
  return true;
}

std::string& Tool::GetTimeSecString() {
  time_t now = time(NULL);

  if (now != Tool::digit_time_.sec_) {
    struct tm base;
    time_t zone_now = now + 28800;
    gmtime_r(&zone_now, &base);
    char buf[32] = { 0 };
    snprintf(buf, 32, "%04d%02d%02d%02d%02d%02d", base.tm_year + 1900,
             base.tm_mon + 1, base.tm_mday, base.tm_hour, base.tm_min,
             base.tm_sec);

    Tool::digit_time_.sec_ = now;
    Tool::digit_time_.digit_string_ = buf;
  }

  return Tool::digit_time_.digit_string_;
}

std::string Tool::GetTimeMicroString(bool blank) {
  struct timeval tv;
  gettimeofday(&tv, NULL);

  char time_string[32] = { 0 };
  if (blank) {
    if (tv.tv_sec != Tool::alpha_time_.sec_) {
      struct tm base;
      time_t zone_now = tv.tv_sec + 28800;
      gmtime_r(&zone_now, &base);
      char buf[32] = { 0 };
      snprintf(buf, 32, "%04d-%02d-%02d %02d:%02d:%02d", base.tm_year + 1900,
               base.tm_mon + 1, base.tm_mday, base.tm_hour, base.tm_min,
               base.tm_sec);

      Tool::alpha_time_.sec_ = tv.tv_sec;
      Tool::alpha_time_.alpha_string_ = buf;
    }
    snprintf(time_string, 32, "%s.%06ld",
             Tool::alpha_time_.alpha_string_.c_str(), tv.tv_usec);
  } else {
    if (tv.tv_sec != Tool::digit_time_.sec_) {
      struct tm base;
      time_t zone_now = tv.tv_sec + 28800;
      gmtime_r(&zone_now, &base);
      char buf[32] = { 0 };
      snprintf(buf, 32, "%04d%02d%02d%02d%02d%02d", base.tm_year + 1900,
               base.tm_mon + 1, base.tm_mday, base.tm_hour, base.tm_min,
               base.tm_sec);

      Tool::digit_time_.sec_ = tv.tv_sec;
      Tool::digit_time_.digit_string_ = buf;
    }
    snprintf(time_string, 32, "%s%06ld",
             Tool::digit_time_.digit_string_.c_str(), tv.tv_usec);
  }

  return time_string;
}

std::string& Tool::LogSevToString(LogSev sev) {
  if (sev >= 0 && sev < sizeof(log_level_)) {
    return Tool::log_level_[sev];
  }

  return Tool::log_level_[invalid_sev];
}

std::string& Tool::LogEnvToString(LogEnv env) {
  if (env >= 0 && env < sizeof(log_env_)) {
    return Tool::log_env_[env];
  }

  return Tool::log_env_[invalid_env];
}

std::string& Tool::GetThreadId() {
  if (thread_id_.empty()) {
    long thread_id = syscall(__NR_gettid);
    char ti[32] = { 0 };
    snprintf(ti, sizeof(ti), "%ld", thread_id);

    thread_id_ = ti;
  }

  return thread_id_;
}

}
}

