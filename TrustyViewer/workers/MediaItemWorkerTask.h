#pragma once

#include <memory>
#include <vector>
#include "../MediaItem.h"

namespace realn {

  class MediaItemWorkerTask {
  public:
    enum class TaskState {
      New = 0,
      Waiting = 1,
      Completed = 2,
    };
    using task_id = size_t;
    using task_id_vec_t = std::vector<task_id>;
    using ptr_t = std::shared_ptr<MediaItemWorkerTask>;

    void update() {
      state = waitingTaskIds.empty() ? TaskState::Completed : TaskState::Waiting;
    }

    task_id id = 0;
    QString itemPath;
    TaskState state = TaskState::New;
    MediaItem::ptr_t item;
    task_id_vec_t waitingTaskIds;
  };

}