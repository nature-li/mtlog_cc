// tool.h

#ifndef TOOL_H_
#define TOOL_H_

#include <time.h>
#include <string>
#include <sys/time.h>
#include <pthread.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ad_menu.h"

namespace mtad {
namespace adlog {

struct TmTime {
  TmTime()
      : sec_(0) {
  }

  time_t sec_;
  struct tm tm_;
};

struct DigitTime {
  DigitTime()
      : sec_(0) {

  }

  time_t sec_;
  std::string digit_string_;
};

struct AlphaTime {
  AlphaTime()
      : sec_(0) {
  }

  time_t sec_;
  std::string alpha_string_;
};

class Tool {
 public:
  Tool();
  virtual ~Tool();

  static void GetCurTime(struct tm& base);

  static bool GetTimeFromString(const std::string& str, time_t& when);

  static std::string& GetTimeSecString();

  static std::string GetTimeMicroString(bool blank = true);

  static std::string& LogSevToString(LogSev sev);

  static std::string& LogEnvToString(LogEnv sev);

  static std::string& GetThreadId();

 private:
  static std::string log_level_[];
  static std::string log_env_[];
  static std::string log_keyword_[];

  static thread_local TmTime tm_time_;
  static thread_local DigitTime digit_time_;
  static thread_local AlphaTime alpha_time_;
  static thread_local std::string thread_id_;
};

}
}
#endif /* TOOL_H_ */
