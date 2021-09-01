#pragma once

#include <QSettings>

class Settings : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString hostname MEMBER _hostname NOTIFY hostnameChanged)
  Q_PROPERTY(QString port MEMBER _port NOTIFY portChanged)
  Q_PROPERTY(QString lang MEMBER _lang NOTIFY langChanged)
  Q_PROPERTY(QString baseUrl MEMBER _baseUrl NOTIFY baseUrlChanged)

  QString _hostname = "http://127.0.0.1";
  QString _port = "4567";
  QString _baseUrl = "";
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

  auto& baseUrl() {
    return _baseUrl;
  }

signals:
  void hostnameChanged();
  void portChanged();
  void langChanged();
  void baseUrlChanged();
};
