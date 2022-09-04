#pragma once

#include <QSettings>

class Settings : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString hostname MEMBER _hostname NOTIFY hostnameChanged)
  Q_PROPERTY(QString lang MEMBER _lang NOTIFY langChanged)
  Q_PROPERTY(bool nsfw MEMBER _nsfw NOTIFY nsfwChanged)
  Q_PROPERTY(bool lightTheme MEMBER _lightTheme NOTIFY lightThemeChanged)
  Q_PROPERTY(QString username MEMBER _username NOTIFY usernameChanged)
  Q_PROPERTY(QString password MEMBER _password NOTIFY passwordChanged)

  QString _hostname = "http://127.0.0.1";
  QString _lang;
  bool _nsfw = false;
  bool _lightTheme = false;
  QString _username;
  QString _password;

  QSettings _settings;

  void save();
  void load();

public:
  enum Viewer {
    WEBTOON,
    MANGA,
  };
  Q_ENUM(Viewer)

  Settings();
  ~Settings();

  auto& hostname() {
    return _hostname;
  }

  auto& username() {
    return _username;
  }

  auto& password() {
    return _password;
  }

  auto& nsfw() {
    return _nsfw;
  }

signals:
  void hostnameChanged();
  void langChanged();
  void nsfwChanged();
  void lightThemeChanged();
  void usernameChanged();
  void passwordChanged();
};
