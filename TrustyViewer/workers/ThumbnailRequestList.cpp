
#include "ThumbnailRequestList.h"

namespace realn {
  ThumbnailRequestList::ThumbnailRequestList() = default;

  void ThumbnailRequestList::addRequest(const QString & item)
  {
    auto lock = std::unique_lock<std::mutex>(mutex);
    requests.push_back(item);
    empty = false;
    condition.notify_one();
  }

  QString ThumbnailRequestList::popRequest()
  {
    auto lock = std::unique_lock<std::mutex>(mutex);
    auto filepath = requests.first();
    requests.pop_front();
    empty = requests.isEmpty();
    return filepath;
  }

  void ThumbnailRequestList::clearRequests()
  {
    auto lock = std::unique_lock<std::mutex>(mutex);
    requests.clear();
    empty = true;
  }

  void ThumbnailRequestList::waitForRequests()
  {
    auto lock = std::unique_lock<std::mutex>(mutex);
    condition.wait(lock);
  }

  void ThumbnailRequestList::wakeAll()
  {
    condition.notify_all();
  }
}