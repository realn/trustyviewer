#pragma once

#include <memory>
#include <mutex>
#include <atomic>

#include <QString>
#include <QPixmap>

namespace realn {
  class ThumbnailDoneList {
  public:
    class JobDone {
    public:
      QString filePath;
      std::unique_ptr<QPixmap> thumbnail;
    };
    using done_vec_t = std::vector<JobDone>;

    ThumbnailDoneList();

    void addDone(const QString& filePath, std::unique_ptr<QPixmap> thumbnail);
    done_vec_t popAllDone();

    bool hasDone() const { return done; }

  private:
    done_vec_t completed;
    std::mutex mutex;
    std::atomic_bool done = false;
  };
}