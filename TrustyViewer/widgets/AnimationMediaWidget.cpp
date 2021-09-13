
#include <QBoxLayout>

#include "AnimationMediaWidget.h"

namespace realn {
  AnimationMediaWidget::AnimationMediaWidget()
  {
    animationPlayer = new QLabel();
    animationPlayer->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    animationPlayer->setAlignment(Qt::AlignCenter);

    auto layout = new QHBoxLayout();
    layout->addWidget(animationPlayer);
    setLayout(layout);
  }

  bool AnimationMediaWidget::loadMedia(MediaItem::ptr_t mediaItem, std::shared_ptr<ExtPlugin> plugin)
  {
    if (!mediaItem || !plugin)
      return false;

    auto newMovie = plugin->loadAnimation(mediaItem->getFilePath());
    if (!newMovie)
      return false;

    movie = std::move(newMovie);
    animationPlayer->setMovie(movie.get());

    movie->start();

    return true;
  }

  void AnimationMediaWidget::clearMedia()
  {
    if (movie)
      movie->stop();
    animationPlayer->setMovie(nullptr);
    movie.reset();
  }
}