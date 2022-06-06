#pragma once

#include <memory>

#include "../MediaItem.h"

class QPixmap;

namespace realn {
  class ThumbnailModelItem : public std::enable_shared_from_this<ThumbnailModelItem> {
  public:
    using ptr_t = std::shared_ptr<ThumbnailModelItem>;
    using pixmap_ptr_t = std::unique_ptr<QPixmap>;
    using itemsorter_t = std::function<bool(ptr_t&, ptr_t&)>;

    static const itemsorter_t AscTypeNameSorter;

    ThumbnailModelItem(MediaItem::ptr_t item);
    ~ThumbnailModelItem();

    MediaItem::ptr_t getMediaItem() const;

    const QString& getName() const;
    const QString& getFilepath() const;
    MediaItemType getType() const;

    bool isDirectory() const;

    void setThumbnail(pixmap_ptr_t pixmap);
    const QPixmap& getThumbnail() const;
    bool hasThumbnail() const;

    ThumbnailModelItem::ptr_t getPtr();

    void setIndex(size_t index);
    size_t getIndex() const;

  private:
    MediaItem::ptr_t mediaItem;
    QString name;
    QString filepath;
    MediaItemType type;
    pixmap_ptr_t thumbnail;
    size_t index = 0;
  };
}