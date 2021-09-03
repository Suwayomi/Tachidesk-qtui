#pragma once

#include <QSettings>

class Settings : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString hostname MEMBER _hostname NOTIFY hostnameChanged)
  Q_PROPERTY(QString lang MEMBER _lang NOTIFY langChanged)

  QString _hostname = "http://127.0.0.1";
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

signals:
  void hostnameChanged();
  void langChanged();
};
