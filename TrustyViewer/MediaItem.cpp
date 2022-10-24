
#include <QDir>
#include <QFileInfo>

#include "MediaItem.h"

namespace realn {
  static const auto MEDIA_ITEM_PATH_SEP = QString("/");

  const MediaItem::itemsorter_t MediaItem::AscTypeNameSorter = MediaItem::itemsorter_t([](MediaItem::ptr_t& itemA, MediaItem::ptr_t& itemB) -> bool {
    if (itemA->getType() == itemB->getType())
      return QString::localeAwareCompare(itemA->getName(), itemB->getName()) < 0;
    return itemA->getType() < itemB->getType();
                                                                                       });

  MediaItem::MediaItem(const QString& filepath, MediaItemType type)
    : filepath(filepath)
    , type(type) {
  }

  bool MediaItem::isDirectory() const { return type == MediaItemType::Directory; }

  QString MediaItem::getName() const {
    if (isDirectory())
      return QDir(filepath).dirName();
    return QFileInfo(filepath).fileName();
  }

  std::shared_ptr<MediaItem> MediaItem::getParent() const {
    return parent.lock();
  }

  bool MediaItem::hasInPathToRoot(MediaItem::ptr_t item) const {
    if (shared_from_this() == item)
      return true;
    if (parent.expired())
      return false;

    return parent.lock()->hasInPathToRoot(item);
  }

  size_t MediaItem::getIndexFromParent() const {
    auto& list = getParent()->getChildren();
    auto it = std::find(list.begin(), list.end(), shared_from_this());
    return static_cast<size_t>(it - list.begin());
  }

  void MediaItem::replaceFilePath(const QString& filePath) {
    filepath = filePath;
  }

  void MediaItem::updateFilePath() {
    auto parent = getParent();
    if (parent != nullptr) {
      auto newPath = QDir::cleanPath(parent->getFilePath() + QDir::separator() + getName());
    }
  }

  void MediaItem::setParent(ptr_t item) {
    parent = item;
  }

  void MediaItem::removeChild(ptr_t item) {
    auto it = std::find(children.begin(), children.end(), item);
    if (it != children.end())
      children.erase(it);
  }

  QString MediaItem::getPathToRoot() const {
    if (parent.expired())
      return getName();
    return parent.lock()->getPathToRoot() + MEDIA_ITEM_PATH_SEP + getName();
  }

  void MediaItem::addChild(ptr_t child) {
    if (child->hasParent())
      child->getParent()->removeChild(child);
    child->setParent(shared_from_this());
    children.push_back(child);
  }

  MediaItem::ptr_t MediaItem::getChild(size_t index) const {
    if (index >= children.size())
      return nullptr;
    return children[index];
  }

  bool MediaItem::hasChildren() const {
    return !children.empty();
  }

  void MediaItem::makeOrphan() {
    auto parent = getParent();
    if (parent) {
      setParent(nullptr);

      auto item = shared_from_this();
      parent->removeChild(item);
    }
  }
}