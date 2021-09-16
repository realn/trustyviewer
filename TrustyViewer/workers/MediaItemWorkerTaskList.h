#pragma once

#include <thread>
#include <condition_variable>
#include <mutex>
#include <list>

#include "MediaItemWorkerTask.h"

namespace realn {
  class MediaItemWorkerTaskList {
  public:
    using task_t = MediaItemWorkerTask;
    using task_ptr_t = task_t::ptr_t;
    using task_id = task_t::task_id;

    void push_back(task_ptr_t task);
    task_ptr_t pop_front();

    template<class _Pred>
    task_ptr_t find_if(_Pred pred) {
      auto lock = lock_t(mutex);
      auto it = std::find_if(list.begin(), list.end(), pred);
      if (it == list.end())
        return nullptr;
      return *it;
    }

    template<class _Pred>
    task_ptr_t pop_if(_Pred pred) {
      auto lock = lock_t(mutex);
      auto it = std::find_if(list.begin(), list.end(), pred);
      if (it == list.end())
        return nullptr;
      auto result = *it;
      list.erase(it);
      return result;
    }

    template<class _Pred>
    bool contains_if(_Pred pred) const {
      auto lock = lock_t(mutex);
      return std::find_if(list.begin(), list.end(), pred) != list.end();
    }

    void wakeAll() {
      cond.notify_all();
    }

  private:
    using list_t = std::list<task_ptr_t>;
    using mutex_t = std::mutex;
    using lock_t = std::unique_lock<mutex_t>;

    list_t list;
    mutable mutex_t mutex;
    std::condition_variable cond;
  };
}