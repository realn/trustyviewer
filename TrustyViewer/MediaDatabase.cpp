
#include <QFileInfo>
#include <QDir>

#include "MediaDatabase.h"

namespace realn {
  MediaDatabase::MediaDatabase(std::shared_ptr<ExtPluginList> plugins)
    : plugins(plugins)
  {
  }

  MediaItem::ptr_t MediaDatabase::getRootItem() const
  {
    return rootItem;
  }

  void MediaDatabase::rebuid(const QString& newRootPath)
  {
    emit rebuildingDatabase();
    rootPath = newRootPath;
    mainList.clear();

    rootItem = buildFromPath(rootPath, nullptr);
    emit databaseRebuild();
  }

  MediaItem::ptr_t MediaDatabase::buildFromPath(const QString& path, MediaItem::ptr_t parent)
  {
    auto info = QFileInfo(path);
    if (!info.exists())
      return nullptr;

    if (info.isDir())
      return buildDir(path, parent);

    auto plugin = plugins->getPluginForExt(info.completeSuffix());
    if (!plugin)
      return nullptr;

    if (plugin->isAnimated(info.completeSuffix()))
      return buildVideo(path, parent);

    return buildImage(path, parent);
  }

  MediaItem::ptr_t MediaDatabase::buildDir(const QString& path, MediaItem::ptr_t parent)
  {
    auto dir = QDir(path, "*", QDir::Type | QDir::IgnoreCase);

    auto result = std::make_shared<MediaItem>(parent, path, MediaItemType::Directory);

    for (auto& item : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
      auto newItem = buildFromPath(dir.filePath(item), result);
      if (newItem)
        result->addChild(newItem);
    }
    
    for (auto& item : dir.entryList(getNameFilters(), QDir::Files | QDir::NoDotAndDotDot)) {
      auto newItem = buildFromPath(dir.filePath(item), result);
      if (newItem)
        result->addChild(newItem);
    }

    return result;
  }

  MediaItem::ptr_t MediaDatabase::buildImage(const QString& path, MediaItem::ptr_t parent)
  {
    auto result = std::make_shared<MediaItem>(parent, path, MediaItemType::Image);
    mainList.push_back(result);
    return result;
  }

  MediaItem::ptr_t MediaDatabase::buildVideo(const QString& path, MediaItem::ptr_t parent)
  {
    auto result = std::make_shared<MediaItem>(parent, path, MediaItemType::Video);
    mainList.push_back(result);
    return result;
  }
  QStringList MediaDatabase::getNameFilters() const
  {
    auto result = QStringList();
    for (auto& ext : plugins->getSupportedExts()) {
      result << ("*." + ext);
    }
    return result;
  }
}