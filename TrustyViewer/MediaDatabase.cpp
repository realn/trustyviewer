
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

    rootItem = buildFromPath(rootPath);
    emit databaseRebuild();
  }

  MediaItem::ptr_t MediaDatabase::buildFromPath(const QString& path)
  {
    auto info = QFileInfo(path);
    if (!info.exists())
      return nullptr;

    if (info.isDir())
      return buildDir(path);

    auto plugin = plugins->getPluginForExt(info.completeSuffix());
    if (!plugin)
      return nullptr;

    if (plugin->isVideo(info.completeSuffix()))
      return buildMediaItem(path, MediaItemType::Video);

    if (plugin->isAnimated(info.completeSuffix()))
      return buildMediaItem(path, MediaItemType::Animation);

    return buildMediaItem(path, MediaItemType::Image);
  }

  MediaItem::ptr_t MediaDatabase::buildMediaItem(const QString& path, MediaItemType type)
  {
    auto result = std::make_shared<MediaItem>(path, type);
    mainList.push_back(result);
    return result;
  }

  MediaItem::ptr_t MediaDatabase::buildDir(const QString& path)
  {
    auto result = std::make_shared<MediaItem>(path, MediaItemType::Directory);

    auto dir = QDir(path, "*", QDir::Type | QDir::IgnoreCase);

    for (auto& item : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
      auto newItem = buildFromPath(dir.filePath(item));
      if (newItem)
        result->addChild(newItem);
    }
    
    for (auto& item : dir.entryList(getNameFilters(), QDir::Files | QDir::NoDotAndDotDot)) {
      auto newItem = buildFromPath(dir.filePath(item));
      if (newItem)
        result->addChild(newItem);
    }

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