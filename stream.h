#ifndef STREAM_H_
#define STREAM_H_

#include <string>

namespace mtad {
namespace adlog {

class Stream {
 public:
  virtual ~Stream();

  // char: %c
  virtual Stream& operator<<(char value);

  // const char*: %s
  virtual Stream& operator<<(const char* value);

  // bool: true or false
  virtual Stream& operator<<(bool value);

  // signed char: %hhd
  virtual Stream& operator<<(signed char value);

  // unsigned char: %hhu
  virtual Stream& operator<<(unsigned char value);

  // signed short: %hd
  virtual Stream& operator<<(short value);

  // unsigned short: %hu
  virtual Stream& operator<<(unsigned short value);

  // int: %d
  virtual Stream& operator<<(int value);

  // unsigned int: %u
  virtual Stream& operator<<(unsigned int value);

  // long: %ld
  virtual Stream& operator<<(long value);

  // unsigned long: %lu
  virtual Stream& operator<<(unsigned long value);

  // long long: %lld
  virtual Stream& operator<<(long long value);

  // unsigned long long: %llu
  virtual Stream& operator<<(unsigned long long value);

  // float: %f
  virtual Stream& operator<<(float value);

  // double: %lf
  virtual Stream& operator<<(double value);

  // long double: %Lf
  virtual Stream& operator<<(long double value);

  // string: %s
  virtual Stream& operator<<(std::string& value);

  // const string: %s
  virtual Stream& operator<<(const std::string& value);

  // const void*: %p
  virtual Stream& operator<<(const void* value);

  // std::endl: \n
  virtual Stream& operator<<(std::ostream&(*f)(std::ostream&));
};

}
}

#endif
