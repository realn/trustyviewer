#pragma once

#include <memory>

#include <QString>
#include <QImage>
#include <QMovie>

namespace realn {
  class ExtPlugin {
  public:
    virtual ~ExtPlugin() = default;

    bool isExtSupported(const QString& ext) const { return getSupportedExts().contains(ext.toLower()); }

    virtual QStringList getSupportedExts() const = 0;
    virtual bool isAnimated(const QString& ext) const = 0;

    virtual std::unique_ptr<QImage> loadImage(const QString& filepath) const = 0;
    virtual std::unique_ptr<QMovie> loadMovie(const QString& filepath) const = 0;
  };

  class ExtPluginList {
  public:
    using ExtPluginPtr = std::shared_ptr<ExtPlugin>;

    void addPlugin(ExtPluginPtr plugin);
    ExtPluginPtr getPluginForExt(const QString& ext);

    QStringList getSupportedExts() const;

  private:
    std::vector<ExtPluginPtr> list;
  };


}