#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>

namespace realn {
  class ThreadWorker {
  public:
    ThreadWorker();
    virtual ~ThreadWorker();

  protected:
    virtual bool isTerminated() const = 0;
    virtual bool isJobPresent() const = 0;
    virtual void Run() = 0;

  private:
    void Execute();

    std::thread jobThread;
    std::mutex jobMutex;
    std::condition_variable jobCondition;
  };
}
