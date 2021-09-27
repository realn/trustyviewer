#pragma once

#include <vector>
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
    using task_vec_t = std::vector<task_ptr_t>;
    using task_id_vec_t = std::vector<task_id>;

    void push_back(task_ptr_t task);
    task_ptr_t pop_front();

    task_ptr_t find(task_id taskId) const;

    task_ptr_t popById(task_id taskId);

    task_vec_t popByIdList(task_id_vec_t& idList);

    bool contains(task_id taskId) const;

    void wakeAll() {
      cond.notify_all();
    }

  private:
    using list_t = std::list<task_ptr_t>;
    using mutex_t = std::mutex;
    using lock_t = std::unique_lock<mutex_t>;

    list_t::iterator findById(task_id taskId);

    list_t list;
    mutable mutex_t mutex;
    std::condition_variable cond;
  };
}