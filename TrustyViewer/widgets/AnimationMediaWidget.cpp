
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

  void AnimationMediaWidget::setMovie(std::unique_ptr<QMovie> value)
  {
    movie = std::move(value);

    movie->stop();
    animationPlayer->setMovie(movie.get());
    movie->start();
  }

  void AnimationMediaWidget::clearMovie()
  {
    if (movie)
      movie->stop();
    animationPlayer->setMovie(nullptr);
    movie.reset();
  }
}