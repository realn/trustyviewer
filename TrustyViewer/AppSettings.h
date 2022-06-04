#pragma once

#include <QSize>
#include <QPoint>
#include <QSettings>

class QWidget;
class QMainWindow;

namespace realn {
  class AppSettings {
  public:
    AppSettings();

    void readWindow(const QString& section, QWidget* widget);
    void writeWindow(const QString& section, QWidget* widget);

    void readMainWindow(const QString& section, QMainWindow* window);
    void writeMainWindow(const QString& section, QMainWindow* window);

    QPoint readPoint(const QString& section, const QString& nameprefix, const QPoint& defPoint = QPoint(0,0));
    void writePoint(const QString& section, const QString& nameprefix, const QPoint& value);

    QSize readSize(const QString& section, const QString& nameprefix, const QSize& defSize = QSize(100, 100));
    void writeSize(const QString& section, const QString& nameprefix, const QSize& value);

    int readInt(const QString& section, const QString& name, int defValue = 0);
    void writeInt(const QString& section, const QString& name, int value);

  private:
    QSettings settings;
  };
}