
#include <QPixmap>

#include "ThumbnailModelItem.h"

namespace realn {
  const ThumbnailModelItem::itemsorter_t ThumbnailModelItem::AscTypeNameSorter = ThumbnailModelItem::itemsorter_t([](ThumbnailModelItem::ptr_t& itemA, ThumbnailModelItem::ptr_t& itemB) -> bool {
    if (itemA->getType() == itemB->getType())
      return QString::localeAwareCompare(itemA->getName(), itemB->getName()) < 0;
    return itemA->getType() < itemB->getType();
                                                                                                                  });

  ThumbnailModelItem::ThumbnailModelItem(MediaItem::ptr_t item)
    : mediaItem(item) {
    name = mediaItem->getName();
    filepath = mediaItem->getFilePath();
    type = mediaItem->getType();
  }

  ThumbnailModelItem::~ThumbnailModelItem() = default;

  MediaItem::ptr_t ThumbnailModelItem::getMediaItem() const {
    return mediaItem;
  }

  const QString& ThumbnailModelItem::getName() const {
    return name;
  }

  const QString& ThumbnailModelItem::getFilepath() const {
    return filepath;
  }

  MediaItemType ThumbnailModelItem::getType() const {
    return type;
  }

  bool ThumbnailModelItem::isDirectory() const {
    return type == MediaItemType::Directory;
  }

  void ThumbnailModelItem::setThumbnail(pixmap_ptr_t pixmap) {
    thumbnail = std::move(pixmap);
  }

  const QPixmap& ThumbnailModelItem::getThumbnail() const {
    return *thumbnail;
  }

  bool ThumbnailModelItem::hasThumbnail() const {
    return thumbnail != nullptr;
  }

  ThumbnailModelItem::ptr_t ThumbnailModelItem::getPtr() {
    return shared_from_this();
  }

}
