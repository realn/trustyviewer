#include "MediaItemStorage.h"


namespace realn {
  MediaItemStorage::item_id MediaItemStorage::store(MediaItem::ptr_t item) {
    auto result = ++idCounter;
    items[result] = item;

    return result;
  }

  MediaItem::ptr_t MediaItemStorage::retrieve(item_id id) {
    auto it = items.find(id);
    if (it == items.end())
      return nullptr;

    auto item = it->second;
    items.erase(it);

    if (item.expired())
      return nullptr;
    return item.lock();
  }

}
