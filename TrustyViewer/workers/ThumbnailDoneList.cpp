
#include "ThumbnailDoneList.h"

namespace realn {
  ThumbnailDoneList::ThumbnailDoneList() = default;

  void ThumbnailDoneList::addDone(const QString& filePath, std::unique_ptr<QPixmap> thumbnail)
  {
    auto lock = std::unique_lock<std::mutex>(mutex);
    completed.push_back({ filePath, std::move(thumbnail) });
    done = true;
  }

  ThumbnailDoneList::done_vec_t ThumbnailDoneList::popAllDone()
  {
    auto lock = std::unique_lock<std::mutex>(mutex);
    auto result = done_vec_t();
    std::swap(completed, result);
    done = false;
    return result;
  }
}