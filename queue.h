// queue.h
#ifndef QUEUE_H_
#define QUEUE_H_

namespace mtad {
namespace adlog {

template<typename T>
class Queue {
 public:
  Queue(int total) {
    // TODO Auto-generated constructor stub
    header_ = 0;
    tail_ = 0;
    size_ = 0;
    total_ = total;
    queue_ = new T[total_];
  }

  virtual ~Queue() {
    if (queue_) {
      delete[] queue_;
    }
  }

  bool Full() {
    return header_ == (tail_ + 1) % total_;
  }

  bool Empty() {
    return header_ == tail_;
  }

  int Size() {
    return size_;
  }

  T Get(int index) throw (std::bad_exception) {
    if (Empty()) {
      throw std::bad_exception();
    }

    index = (header_ + index) % total_;
    return queue_[index];
  }

  bool Push(const T& value) {
    if (Full()) {
      return false;
    }
    queue_[tail_] = value;
    tail_ = (tail_ + 1) % total_;
    size_++;
    return true;
  }

  T Pop() throw (std::bad_exception) {
    if (Empty()) {
      throw std::bad_exception();
    }

    T value = queue_[header_];
    header_ = (header_ + 1) % total_;
    size_--;
    return value;
  }

 private:
  int header_;
  int tail_;
  int size_;
  int total_;
  T* queue_;
};

}
}
#endif /* QUEUE_H_ */
