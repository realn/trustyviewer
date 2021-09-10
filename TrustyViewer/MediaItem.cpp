
#include "MediaItem.h"

namespace realn {
  MediaItem::MediaItem(const QString& filepath, MediaItemType type) 
    : filepath(filepath)
    , type(type)
  {
  }

  std::shared_ptr<MediaItem> MediaItem::getParent() const
  {
    return parent.lock();
  }

  void MediaItem::setParent(ptr_t item)
  {
    parent = item;
  }

  void MediaItem::addChild(ptr_t child)
  {
    assert(!child->hasParent());
    child->setParent(shared_from_this());
    children.push_back(child);
  }

  MediaItem::ptr_t MediaItem::getChild(size_t index) const
  {
    if (index >= children.size())
      return nullptr;
    return children[index];
  }
}