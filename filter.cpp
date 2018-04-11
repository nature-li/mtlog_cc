// filter.cpp

#include "filter.h"

namespace mtad {
namespace adlog {
Filter::Filter()
    : min_level_(0) {
}

Filter::~Filter() {
}

bool Filter::Filt(int level) {
  if (level >= min_level_) {
    return true;
  }

  return false;
}

void Filter::SetLevel(int level) {
  min_level_ = level;
}

}
}

