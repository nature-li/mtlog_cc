// filter.h

#ifndef FILTER_H_
#define FILTER_H_

#include <pthread.h>

namespace mtad {
namespace adlog {

class Filter {
 public:
  Filter();

  ~Filter();

  bool Filt(int level);

  void SetLevel(int level);

 private:
  int min_level_;
};

}
}

#endif /* FILTER_H_ */
