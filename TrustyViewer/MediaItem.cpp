
#include "MediaItem.h"

namespace realn {
  MediaItem::MediaItem(MediaItem::ptr_t parent, const QString& filepath, MediaItemType type) 
    : parent(parent)
    , filepath(filepath)
    , type(type)
  {
  }

  std::shared_ptr<MediaItem> MediaItem::getParent() const
  {
    return parent.lock();
  }

  void MediaItem::addChild(ptr_t child)
  {
    children.push_back(child);
  }
}