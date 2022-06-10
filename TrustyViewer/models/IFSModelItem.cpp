
#include <QFileInfo>

#include "../Utils.h"
#include "IFSModelItem.h"

namespace realn {
  const IFSModelItem::itemsorter_t IFSModelItem::AscTypeNameSorter = IFSModelItem::itemsorter_t([](IFSModelItem::ptr_t& itemA, IFSModelItem::ptr_t& itemB) -> bool {
    if (itemA->getType() == itemB->getType())
      return QString::localeAwareCompare(itemA->getName(), itemB->getName()) < 0;
    return itemA->getType() < itemB->getType();
                                                                                                });

  const IFSModelItem::ptr_t IFSModelItem::itemNull = nullptr;

  IFSModelItem::IFSModelItem(MediaItem::ptr_t item, QFileIconProvider& iconProvider)
    : mediaItem(item) {
    assert(item != nullptr);

    auto info = QFileInfo(item->getFilePath());

    name = info.fileName();
    icon = iconProvider.icon(info);
    type = item->getType();
  }

  IFSModelItem::~IFSModelItem() = default;

  const QString& IFSModelItem::getName() const {
    return name;
  }

  const QIcon& IFSModelItem::getIcon() const {
    return icon;
  }

  MediaItem::ptr_t IFSModelItem::getMediaItem() {
    return mediaItem;
  }

  MediaItemType IFSModelItem::getType() const {
    return type;
  }

  bool IFSModelItem::isDirectory() const {
      return type == MediaItemType::Directory;
  }

  IFSModelItem::ptr_t IFSModelItem::getParent() const {
    if (parent.expired())
      return nullptr;
    return parent.lock();
  }

  bool IFSModelItem::hasParent() const {
    return !parent.expired();
  }

  void IFSModelItem::addChild(IFSModelItem::ptr_t item) {
    auto it = find(children, item);
    if (it != children.end())
      return;

    if (item->hasParent())
      item->getParent()->removeChild(item);
    item->parent = shared_from_this();
    children.push_back(item);
  }

  void IFSModelItem::removeChild(IFSModelItem::ptr_t item) {
    auto it = find(children, item);
    if (it == children.end())
      return;

    item->parent.reset();
    children.erase(it);
  }

  const IFSModelItem::list_t& IFSModelItem::getChildren() const {
    return children;
  }

  size_t IFSModelItem::getNumberOfChildren() const {
    return children.size();
  }

  IFSModelItem::ptr_t IFSModelItem::getChild(size_t index) const {
    if (index >= children.size())
      return nullptr;
    return children.at(index);
  }

  IFSModelItem::ptr_t IFSModelItem::getPtr() {
    return shared_from_this();
  }
}
