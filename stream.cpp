#include "stream.h"

namespace mtad {
namespace adlog {

Stream::~Stream() {
}

// char: %c
Stream& Stream::Stream::operator<<(char value) {
  return *this;
}

// const char*: %s
Stream& Stream::operator<<(const char* value) {
  return *this;
}

// bool: true or false
Stream& Stream::operator<<(bool value) {
  return *this;
}

// signed char: %hhd
Stream& Stream::operator<<(signed char value) {
  return *this;
}

// unsigned char: %hhu
Stream& Stream::operator<<(unsigned char value) {
  return *this;
}

// signed short: %hd
Stream& Stream::operator<<(short value) {
  return *this;
}

// unsigned short: %hu
Stream& Stream::operator<<(unsigned short value) {
  return *this;
}

// int: %d
Stream& Stream::operator<<(int value) {
  return *this;
}

// unsigned int: %u
Stream& Stream::operator<<(unsigned int value) {
  return *this;
}

// long: %ld
Stream& Stream::operator<<(long value) {
  return *this;
}

// unsigned long: %lu
Stream& Stream::operator<<(unsigned long value) {
  return *this;
}

// long long: %lld
Stream& Stream::operator<<(long long value) {
  return *this;
}

// unsigned long long: %llu
Stream& Stream::operator<<(unsigned long long value) {
  return *this;
}

// float: %f
Stream& Stream::operator<<(float value) {
  return *this;
}

// double: %lf
Stream& Stream::operator<<(double value) {
  return *this;
}

// long double: %Lf
Stream& Stream::operator<<(long double value) {
  return *this;
}

// string: %s
Stream& Stream::operator<<(std::string& value) {
  return *this;
}

// const string: %s
Stream& Stream::operator<<(const std::string& value) {
  return *this;
}

// const void*: %p
Stream& Stream::operator<<(const void* value) {
  return *this;
}

// std::endl: \n
Stream& Stream::operator<<(std::ostream&(*f)(std::ostream&)) {
  return *this;
}

}
}
