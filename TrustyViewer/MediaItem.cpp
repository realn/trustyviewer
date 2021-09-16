
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

  size_t MediaItem::getIndexFromParent() const
  {
    auto& list = getParent()->getChildren();
    auto it = std::find(list.begin(), list.end(), shared_from_this());
    return static_cast<size_t>(it - list.begin());
  }

  void MediaItem::setParent(ptr_t item)
  {
    parent = item;
  }

  void MediaItem::removeChild(ptr_t item)
  {
    auto it = std::find(children.begin(), children.end(), item);
    if(it != children.end())
      children.erase(it);
  }

  void MediaItem::addChild(ptr_t child)
  {
    if (child->hasParent())
      child->getParent()->removeChild(child);
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