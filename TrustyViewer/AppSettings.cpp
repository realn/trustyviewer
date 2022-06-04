
#include <QWidget>
#include <QMainWindow>

#include "AppSettings.h"

namespace realn {
  QString iniPath(const QString& section, const QString& name) {
    return section + "/" + name;
  }

  QString iniNameValue(const QString& namePrefix, const QString& name) {
    return namePrefix + "-" + name;
  }

  AppSettings::AppSettings()
    : settings("settings.ini", QSettings::IniFormat)
  {

  }

  void AppSettings::readWindow(const QString& section, QWidget* widget) {
    auto val = settings.value(iniPath(section, "geometry"), widget->saveGeometry());
    widget->restoreGeometry(val.toByteArray());
  }

  void AppSettings::writeWindow(const QString& section, QWidget* widget) {
    settings.setValue(iniPath(section, "geometry"), widget->saveGeometry());
  }

  void AppSettings::readMainWindow(const QString& section, QMainWindow* window) {
    readWindow(section, window);
    auto val = settings.value(iniPath(section, "state"), window->saveState());
    window->restoreState(val.toByteArray());
  }

  void AppSettings::writeMainWindow(const QString& section, QMainWindow* window) {
    writeWindow(section, window);
    settings.setValue(iniPath(section, "state"), window->saveState());
  }

  QPoint AppSettings::readPoint(const QString& section, const QString& nameprefix, const QPoint& defPoint) {
    int x = readInt(section, iniNameValue(nameprefix, "x"), defPoint.x());
    int y = readInt(section, iniNameValue(nameprefix, "y"), defPoint.y());
    return QPoint(x, y);
  }

  void AppSettings::writePoint(const QString& section, const QString& nameprefix, const QPoint& value) {
    writeInt(section, iniNameValue(nameprefix, "x"), value.x());
    writeInt(section, iniNameValue(nameprefix, "y"), value.y());
  }

  QSize AppSettings::readSize(const QString& section, const QString& nameprefix, const QSize& defSize) {
    int width = readInt(section, iniNameValue(nameprefix, "width"), defSize.width());
    int height = readInt(section, iniNameValue(nameprefix, "height"), defSize.height());
    return QSize(width, height);
  }

  void AppSettings::writeSize(const QString& section, const QString& nameprefix, const QSize& value) {
    writeInt(section, iniNameValue(nameprefix, "width"), value.width());
    writeInt(section, iniNameValue(nameprefix, "height"), value.height());
  }

  int AppSettings::readInt(const QString& section, const QString& name, int defValue) {
    return settings.value(iniPath(section, name), defValue).toInt();
  }

  void AppSettings::writeInt(const QString& section, const QString& name, int value) {
    settings.setValue(iniPath(section, name), value);
  }

}
