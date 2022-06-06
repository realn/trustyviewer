#pragma once

#include <map>

#include "MediaItem.h"

namespace realn {
  class MediaItemStorage {
  public:
    using item_id = size_t;

    item_id store(MediaItem::ptr_t item);
    MediaItem::ptr_t retrieve(item_id id);

  private:
    using item_map_t = std::map<item_id, MediaItem::weak_ptr_t>;

    item_map_t items;
    item_id idCounter = 0;
  };
}
