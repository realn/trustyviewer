#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include <QStringList>

namespace realn {
  class ThumbnailRequestList {
  public:
    ThumbnailRequestList();

    void addRequest(const QString& item);
    QString popRequest();
    void clearRequests();

    void wakeAll();

  private:
    QStringList requests;
    std::mutex mutex;
    std::condition_variable condition;
  };
}