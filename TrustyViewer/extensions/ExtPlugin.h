#pragma once

#include <memory>

#include <QString>
#include <QImage>
#include <QMovie>
#include <QtMultimedia/QMediaContent>

namespace realn {
  class ExtPlugin {
  public:
    enum class Errors {
      NoError = 0,
      FailedToLoad,
      PluginBusy
    };

    using pixmapptr_t = std::unique_ptr<QPixmap>;
    struct result_t;

    virtual ~ExtPlugin() = default;

    bool isExtSupported(const QString& ext) const { return getSupportedExts().contains(ext.toLower()); }

    virtual QStringList getSupportedExts() const = 0;
    virtual bool isAnimated(const QString& ext) const { return false; }
    virtual bool isVideo(const QString& ext) const { return false; }

    virtual std::unique_ptr<QImage> loadImage(const QString& filepath) const { return nullptr; }
    virtual std::unique_ptr<QMovie> loadAnimation(const QString& filepath) const { return nullptr; }
    virtual std::unique_ptr<QMediaContent> loadVideo(const QString& filepath) const { return nullptr; }

    virtual result_t createThumbnail(const QString& filepath, QSize size) const = 0;

    struct result_t {
      pixmapptr_t thumbnail;
      Errors error = Errors::NoError;
    };
  };

  class ExtPluginList {
  public:
    using ExtPluginPtr = std::shared_ptr<ExtPlugin>;

    void addPlugin(ExtPluginPtr plugin);
    template<class _T, class = std::enable_if_t<std::is_base_of_v<ExtPlugin, _T>>>
    void addPlugin() { addPlugin(std::make_shared<_T>()); }

    ExtPluginPtr getPluginForExt(const QString& ext);

    QStringList getSupportedExts() const;

  private:
    std::vector<ExtPluginPtr> list;
  };


}