// ad_menu.h

#ifndef AD_MENU_H_
#define AD_MENU_H_

#include <iostream>

namespace mtad {
namespace adlog {

// If adding LogKeyWord, don't forget overwrite operator<< method. If not you will
// not get the right string when logging.
enum LogSev {
  trace,
  debug,
  info,
  warn,
  error,
  fatal,
  report,
  invalid_sev,
};

// If adding LogKeyWord, don't forget overwrite operator<< method. If not you will
// not get the right string when logging.
enum LogEnv {
  product,
  abtest,
  develop,
  invalid_env,
};

}  // namespace adlog
}  // namespace mtad

#endif /* AD_MENU_H_ */
