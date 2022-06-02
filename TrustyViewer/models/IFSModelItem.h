#pragma once

#include <memory>
#include <set>

#include <QIcon>
#include <QFileIconProvider>

#include "../MediaItem.h"

namespace realn {
  class IFSModelItem : public std::enable_shared_from_this<IFSModelItem> {
  public:
    using ptr_t = std::shared_ptr<IFSModelItem>;
    using weak_ptr_t = std::weak_ptr<IFSModelItem>;
    using list_t = std::vector<ptr_t>;
    using itemsorter_t = std::function<bool(ptr_t&, ptr_t&)>;

    static const itemsorter_t AscTypeNameSorter;
    static const ptr_t itemNull;

    IFSModelItem(MediaItem::ptr_t item, QFileIconProvider& iconProvider);
    ~IFSModelItem();

    const QString& getName() const;
    const QIcon& getIcon() const;
    MediaItem::ptr_t getMediaItem();
    MediaItemType getType() const;

    ptr_t getParent() const;
    bool hasParent() const;

    void addChild(IFSModelItem::ptr_t item);
    void removeChild(IFSModelItem::ptr_t item);

    const IFSModelItem::list_t& getChildren() const;

    size_t getNumberOfChildren() const;
    IFSModelItem::ptr_t getChild(size_t index) const;

    template<class _Pred>
    void sortChildren(_Pred pred) {
      sort_cont(children, pred);
    }

    IFSModelItem::ptr_t getPtr();

  private:
    weak_ptr_t parent;
    list_t children;
    MediaItem::ptr_t mediaItem;
    QString name;
    QIcon icon;
    MediaItemType type = MediaItemType::Directory;
  };
}