// sink.cpp

#include <iostream>
#include <pthread.h>
#include "sink.h"

namespace mtad {
namespace adlog {

Sink::Sink(Backend* writter, size_t max_queue_size, bool async) {
  writter_ = writter;
  queue_idx_ = 0;
  log_queues_[0] = new LogQueue(max_queue_size);
  log_queues_[1] = new LogQueue(max_queue_size);
  flag_ = 0;
  consumer_wait_ = false;
  async_ = async;

  pthread_mutex_init(&mutex_, NULL);
  pthread_cond_init(&cond_, NULL);
}

Sink::~Sink() {
  pthread_mutex_destroy(&mutex_);
  pthread_cond_destroy(&cond_);
  delete log_queues_[0];
  delete log_queues_[1];
}

bool Sink::Start() {
  if (async_) {
    thread_ = std::thread(&Sink::WriteLog, this);
  }
  return true;
}

void Sink::Stop() {
  pthread_mutex_lock(&mutex_);
  flag_ = 1;
  pthread_cond_broadcast(&cond_);
  pthread_mutex_unlock(&mutex_);

  if (async_) {
    thread_.join();
  }
}

void Sink::PushLog(Record* record) {
  pthread_mutex_lock(&mutex_);
  if (flag_ == 1) {
    delete record;
    pthread_mutex_unlock(&mutex_);
    return;
  }

  // handle sync
  if (!async_) {
    writter_->WriteFlushRotate(record);
    delete record;
    pthread_mutex_unlock(&mutex_);
    return;
  }

  // wait queue to be not full
  while (log_queues_[queue_idx_]->Full()) {
    pthread_cond_wait(&cond_, &mutex_);
  }

  if (flag_ == 1) {
    delete record;
    pthread_mutex_unlock(&mutex_);
    return;
  }

  // notify consumer thread if necessary
  log_queues_[queue_idx_]->Push(record);
  if (consumer_wait_) {
    pthread_cond_signal(&cond_);
  }

  pthread_mutex_unlock(&mutex_);
}

Sink::QueuePtr Sink::PopLogs() {
  pthread_mutex_lock(&mutex_);
  // wait if queue is empty
  if (log_queues_[queue_idx_]->Empty()) {
    if (flag_ != 1) {

      struct timespec timeout;
      timeout.tv_sec = time(NULL) + 5;
      timeout.tv_nsec = 0;
      consumer_wait_ = true;
      pthread_cond_timedwait(&cond_, &mutex_, &timeout);
      consumer_wait_ = false;
    }
  }

  // switch log queue
  QueuePtr queue_ptr = log_queues_[queue_idx_];
  queue_idx_ = (queue_idx_ + 1) % 2;

  // notify producer thread if necessary
  if (queue_ptr->Full()) {
    pthread_cond_broadcast(&cond_);
  }
  pthread_mutex_unlock(&mutex_);

  return queue_ptr;
}

void Sink::WriteLog() {
  while (true) {
    // queue_ptr may be empty
    QueuePtr queue_ptr = PopLogs();

    // No matter queue_ptr is empty or not, call this function.
    writter_->Write(queue_ptr);

    // Check and rotate all log files
    writter_->Rotate();

    if (flag_ == 1) {
      pthread_mutex_lock(&mutex_);
      writter_->Write(log_queues_[queue_idx_]);
      pthread_mutex_unlock(&mutex_);
      break;
    }
  }
}

}
}

