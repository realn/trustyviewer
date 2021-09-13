#pragma once

#include <QWidget>
#include <QLabel>
#include <QPointer>
#include <QMovie>

namespace realn {
  class AnimationMediaWidget : public QWidget {
    Q_OBJECT;
  public:
    AnimationMediaWidget();

    void setMovie(std::unique_ptr<QMovie> value);
    void clearMovie();

  private:
    QPointer<QLabel> animationPlayer;

    std::unique_ptr<QMovie> movie;
  };
}