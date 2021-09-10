#pragma once

#include <memory>
#include <vector>

#include <QString>

namespace realn {
  enum class MediaItemType {
    Directory = 0,
    Image = 1,
    Video = 2,
  };

  class MediaItem : public std::enable_shared_from_this<MediaItem> {
  public:
    using ptr_t = std::shared_ptr<MediaItem>;
    using itemvector_t = std::vector<ptr_t>;

    MediaItem(const QString& filepath, MediaItemType type);

    bool isDirectory() const { return type == MediaItemType::Directory; }

    std::shared_ptr<MediaItem> getParent() const;
    QString getFilePath() const { return filepath; }
    MediaItemType getType() const { return type; }
    const itemvector_t& getChildren() const { return children; }

    void addChild(ptr_t child);
    ptr_t getChild(size_t index) const;

    ptr_t getPtr() { return shared_from_this(); }

    bool hasParent() const { return parent.lock() != nullptr; }

  private:
    void setParent(ptr_t item);

    std::weak_ptr<MediaItem> parent;
    QString filepath;
    MediaItemType type = MediaItemType::Image;
    itemvector_t children;
  };
}