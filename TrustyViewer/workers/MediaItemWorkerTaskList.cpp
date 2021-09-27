
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

  MediaItemWorkerTaskList::task_ptr_t MediaItemWorkerTaskList::find(task_id taskId) const {
    auto lock = lock_t(mutex);
    auto it = std::find_if(list.begin(), list.end(), [&](const task_ptr_t& item) { return item->id == taskId; });
    if (it == list.end())
      return nullptr;
    return *it;
  }

  MediaItemWorkerTaskList::task_ptr_t MediaItemWorkerTaskList::popById(task_id taskId) {
    auto lock = lock_t(mutex);
    auto it = findById(taskId);
    if (it == list.end())
      return nullptr;
    auto result = *it;
    list.erase(it);
    return result;
  }

  MediaItemWorkerTaskList::task_vec_t MediaItemWorkerTaskList::popByIdList(task_id_vec_t& idList)
  {
    auto lock = lock_t(mutex);
    auto it = idList.begin();
    auto result = task_vec_t();
    while (it != idList.end()) {
      auto taskIt = findById(*it);
      if (taskIt == list.end()) {
        it++;
        continue;
      }

      result.push_back(*taskIt);
      list.erase(taskIt);
      it = idList.erase(it);
    }

    return result;
  }

  bool MediaItemWorkerTaskList::contains(task_id taskId) const {
    auto lock = lock_t(mutex);
    return std::find_if(list.begin(), list.end(), [&](const task_ptr_t& item) {return item->id == taskId; }) != list.end();
  }

  MediaItemWorkerTaskList::list_t::iterator MediaItemWorkerTaskList::findById(task_id taskId)
  {
    return std::find_if(list.begin(), list.end(), [&](task_ptr_t& item) { return item->id == taskId; });
  }
}