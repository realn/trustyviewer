
#include "ThumbnailRequestList.h"

namespace realn {
  ThumbnailRequestList::ThumbnailRequestList() = default;

  void ThumbnailRequestList::addRequest(const QString & item)
  {
    auto lock = std::unique_lock<std::mutex>(mutex);
    requests.push_back(item);
    condition.notify_one();
  }

  QString ThumbnailRequestList::popRequest()
  {
    auto lock = std::unique_lock<std::mutex>(mutex);

    if (requests.empty()) {
      condition.wait(lock);
      if (requests.empty())
        return QString();
    }

    auto filepath = requests.first();
    requests.pop_front();
    return filepath;
  }

  void ThumbnailRequestList::clearRequests()
  {
    auto lock = std::unique_lock<std::mutex>(mutex);
    requests.clear();
  }

  void ThumbnailRequestList::wakeAll()
  {
    condition.notify_all();
  }
}