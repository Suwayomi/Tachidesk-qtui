#pragma once

#include <QSettings>

class Settings : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString hostname MEMBER _hostname NOTIFY hostnameChanged)
  Q_PROPERTY(QString port MEMBER _port NOTIFY portChanged)
  Q_PROPERTY(QString lang MEMBER _lang NOTIFY langChanged)

  QString _hostname = "http://127.0.0.1";
  QString _port = "4567";
  QString _lang;

  QSettings _settings;

  void save();
  void load();

public:
  Settings();
  ~Settings();

  auto& hostname() {
    return _hostname;
  }

  auto& port() {
    return _port;
  }

signals:
  void hostnameChanged();
  void portChanged();
  void langChanged();
};
