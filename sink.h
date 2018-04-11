// sink.h

#ifndef SINK_H_
#define SINK_H_

#include <queue>
#include <thread>
#include <pthread.h>
#include "backend.h"
#include "queue.h"

namespace mtad {
namespace adlog {

class Sink {
  typedef Queue<Record*> LogQueue;
  typedef LogQueue* QueuePtr;
 public:
  Sink(Backend* writter, size_t max_queue_size, bool async);

  ~Sink();

  bool Start();

  void Stop();

  void PushLog(Record* log);

 private:
  QueuePtr PopLogs();

  void WriteLog();

 private:
  // Writter
  Backend* writter_;

  // log queue
  int queue_idx_;
  QueuePtr log_queues_[2];

  // sync for queue
  pthread_mutex_t mutex_;

  // sync for queue
  pthread_cond_t cond_;

  // thread
  std::thread thread_;

  int flag_;

  // consumer thread wait
  bool consumer_wait_;

  // sync or async
  bool async_;
};

}
}

#endif /* SINK_H_ */
