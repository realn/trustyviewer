
#include "ThreadWorker.h"

namespace realn {
  ThreadWorker::ThreadWorker()
  {
    jobThread = std::thread(&ThreadWorker::Execute, this);
  }

  ThreadWorker::~ThreadWorker()
  {
  }

  void ThreadWorker::Execute()
  {
    auto lock = std::unique_lock<std::mutex>(jobMutex);
    jobCondition.wait(lock);

    while (!isTerminated()) {



      Run();

    }
  }
}