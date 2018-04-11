// record.cpp

#include <string.h>
#include <memory>
#include <stdio.h>
#include "record.h"

namespace mtad {
namespace adlog {

std::string Record::sep_ = std::string(1, 0x1e);
std::string Record::log_format_ = "[%s]" + Record::sep_ + "[%s]" + Record::sep_
    + "[%s]" + Record::sep_ + "[%s:%d:%s]" + Record::sep_ + "[%s]"
    + Record::sep_ + "[%s]" + Record::sep_ + "[%s]" + Record::sep_ + "[";
std::string Record::log_tail_ = "]" + Record::sep_ + "\n";

Record::Record()
    : level_(0) {
  content_len_ = 1024;
  content_ = new char[content_len_];
  content_used_ = 0;
}

Record::~Record() {
  if (content_) {
    delete[] content_;
    content_ = nullptr;
  }
}

void Record::Expand(int min) {
  if (min > content_len_ - content_used_) {
    int new_len = content_len_;
    do {
      new_len = new_len * 2;
    } while (min > new_len - content_used_);

    char* new_buf = new char[new_len];
    memcpy((void*) new_buf, content_, content_used_);
    delete[] content_;
    content_ = new_buf;
    content_len_ = new_len;
  }
}

// char: %c
Stream& Record::operator<<(char value) {
  Expand(8);
  char* head = content_ + content_used_;
  int left = content_len_ - content_used_ - 1;
  int added = snprintf(head, left, "%c", value);
  content_used_ += added;
  return *this;
}

// const char*: %s
Stream& Record::operator<<(const char* value) {
  Expand(strlen(value));
  char* head = content_ + content_used_;
  int added = strlen(value);
  memcpy((void*) head, value, added);
  content_used_ += added;
  return *this;
}

// bool: true or false
Stream& Record::operator<<(bool val) {
  std::string value = val ? "true" : "false";
  Expand(value.size());
  char* head = content_ + content_used_;
  int added = value.size();
  memcpy((void*) head, value.c_str(), added);
  content_used_ += added;
  return *this;
}

// signed char: %hhd
Stream& Record::operator<<(signed char value) {
  Expand(8);
  char* head = content_ + content_used_;
  int left = content_len_ - content_used_ - 1;
  int added = snprintf(head, left, "%hhd", value);
  content_used_ += added;
  return *this;
}

// unsigned char: %hhu
Stream& Record::operator<<(unsigned char value) {
  Expand(8);
  char* head = content_ + content_used_;
  int left = content_len_ - content_used_ - 1;
  int added = snprintf(head, left, "%hhu", value);
  content_used_ += added;
  return *this;
}

// signed short: %hd
Stream& Record::operator<<(short value) {
  Expand(16);
  char* head = content_ + content_used_;
  int left = content_len_ - content_used_ - 1;
  int added = snprintf(head, left, "%hd", value);
  content_used_ += added;
  return *this;
}

// unsigned short: %hu
Stream& Record::operator<<(unsigned short value) {
  Expand(16);
  char* head = content_ + content_used_;
  int left = content_len_ - content_used_ - 1;
  int added = snprintf(head, left, "%hu", value);
  content_used_ += added;
  return *this;
}

// int: %d
Stream& Record::operator<<(int value) {
  Expand(32);
  char* head = content_ + content_used_;
  int left = content_len_ - content_used_ - 1;
  int added = snprintf(head, left, "%d", value);
  content_used_ += added;
  return *this;
}

// unsigned int: %u
Stream& Record::operator<<(unsigned int value) {
  Expand(32);
  char* head = content_ + content_used_;
  int left = content_len_ - content_used_ - 1;
  int added = snprintf(head, left, "%u", value);
  content_used_ += added;
  return *this;
}

// long: %ld
Stream& Record::operator<<(long value) {
  Expand(32);
  char* head = content_ + content_used_;
  int left = content_len_ - content_used_ - 1;
  int added = snprintf(head, left, "%ld", value);
  content_used_ += added;
  return *this;
}

// unsigned long: %lu
Stream& Record::operator<<(unsigned long value) {
  Expand(32);
  char* head = content_ + content_used_;
  int left = content_len_ - content_used_ - 1;
  int added = snprintf(head, left, "%lu", value);
  content_used_ += added;
  return *this;
}

// long long: %lld
Stream& Record::operator<<(long long value) {
  Expand(32);
  char* head = content_ + content_used_;
  int left = content_len_ - content_used_ - 1;
  int added = snprintf(head, left, "%lld", value);
  content_used_ += added;
  return *this;
}

// unsigned long long: %llu
Stream& Record::operator<<(unsigned long long value) {
  Expand(32);
  char* head = content_ + content_used_;
  int left = content_len_ - content_used_ - 1;
  int added = snprintf(head, left, "%llu", value);
  content_used_ += added;
  return *this;
}

// float: %f
Stream& Record::operator<<(float value) {
  Expand(64);
  char* head = content_ + content_used_;
  int left = content_len_ - content_used_ - 1;
  int added = snprintf(head, left, "%f", value);
  content_used_ += added;
  return *this;
}

// double: %lf
Stream& Record::operator<<(double value) {
  Expand(64);
  char* head = content_ + content_used_;
  int left = content_len_ - content_used_ - 1;
  int added = snprintf(head, left, "%lf", value);
  content_used_ += added;
  return *this;
}

// long double: %Lf
Stream& Record::operator<<(long double value) {
  Expand(64);
  char* head = content_ + content_used_;
  int left = content_len_ - content_used_ - 1;
  int added = snprintf(head, left, "%Lf", value);
  content_used_ += added;
  return *this;
}

// string: %s
Stream& Record::operator<<(std::string& value) {
  Expand(value.size());
  char* head = content_ + content_used_;
  int added = value.size();
  memcpy((void*) head, value.c_str(), added);
  content_used_ += added;
  return *this;
}

// const string: %s
Stream& Record::operator<<(const std::string& value) {
  Expand(value.size());
  char* head = content_ + content_used_;
  int added = value.size();
  memcpy((void*) head, value.c_str(), added);
  content_used_ += added;
  return *this;
}

// const void*: %p
Stream& Record::operator<<(const void* value) {
  Expand(32);
  char* head = content_ + content_used_;
  int left = content_len_ - content_used_ - 1;
  int added = snprintf(head, left, "%p", value);
  content_used_ += added;
  return *this;
}

// std::endl
Stream& Record::operator<<(std::ostream& (*fun)(std::ostream&)) {
  std::string value = "\n";
  Expand(value.size());
  char* head = content_ + content_used_;
  int added = value.size();
  memcpy((void*) head, value.c_str(), added);
  content_used_ += added;
  return *this;
}

}
}

