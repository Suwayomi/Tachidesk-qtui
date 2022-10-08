#pragma once

#include <QSettings>

class Settings : public QObject {
  Q_OBJECT

public:
  enum Viewer {
    WEBTOON,
    MANGA,
  };
  Q_ENUM(Viewer)

private:
  Q_PROPERTY(QString hostname MEMBER _hostname NOTIFY hostnameChanged)
  Q_PROPERTY(QString lang MEMBER _lang NOTIFY langChanged)
  Q_PROPERTY(bool nsfw MEMBER _nsfw NOTIFY nsfwChanged)
  Q_PROPERTY(bool lightTheme MEMBER _lightTheme NOTIFY lightThemeChanged)
  Q_PROPERTY(bool autoUpdate MEMBER _autoUpdate NOTIFY autoUpdateChanged)
  Q_PROPERTY(QString username MEMBER _username NOTIFY usernameChanged)
  Q_PROPERTY(QString password MEMBER _password NOTIFY passwordChanged)
  Q_PROPERTY(Viewer viewer MEMBER _viewer NOTIFY viewerChanged)

  QString _hostname = "http://127.0.0.1";
  QString _lang;
  bool _nsfw = false;
  bool _lightTheme = false;
  QString _username;
  QString _password;
  Viewer _viewer = Viewer::WEBTOON;
  bool _autoUpdate = false;

  QSettings _settings;

  void save();
  void load();

public:

  Settings();
  ~Settings();

  auto& hostname() const {
    return _hostname;
  }

  auto& username() const {
    return _username;
  }

  auto& password() const {
    return _password;
  }

  auto& nsfw() const {
    return _nsfw;
  }

  auto& autoUpdate() const {
    return _autoUpdate;
  }

signals:
  void hostnameChanged();
  void langChanged();
  void nsfwChanged();
  void lightThemeChanged();
  void usernameChanged();
  void passwordChanged();
  void viewerChanged();
  void autoUpdateChanged();
};
