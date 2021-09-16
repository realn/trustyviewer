
#include "MediaItemWorkerTaskList.h"

namespace realn {
  void MediaItemWorkerTaskList::push_back(task_ptr_t task)
  {
    assert(task);
    auto lock = lock_t(mutex);
    list.push_back(task);
    cond.notify_one();
  }

  MediaItemWorkerTaskList::task_ptr_t MediaItemWorkerTaskList::pop_front()
  {
    auto lock = lock_t(mutex);
    if (list.empty())
      cond.wait(lock);
    if (list.empty())
      return nullptr;

    auto result = list.front();
    assert(result);
    list.pop_front();
    return result;
  }
}