#pragma once

#include <QSettings>

class Settings : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString hostname MEMBER _hostname NOTIFY hostnameChanged)
  Q_PROPERTY(QString lang MEMBER _lang NOTIFY langChanged)
  Q_PROPERTY(bool nsfw MEMBER _nsfw NOTIFY nsfwChanged)
  Q_PROPERTY(bool lightTheme MEMBER _lightTheme NOTIFY lightThemeChanged)

  QString _hostname = "http://127.0.0.1";
  QString _lang;
  bool _nsfw = false;
  bool _lightTheme = false;

  QSettings _settings;

  void save();
  void load();

public:
  Settings();
  ~Settings();

  auto& hostname() {
    return _hostname;
  }

  auto& nsfw() {
    return _nsfw;
  }

signals:
  void hostnameChanged();
  void langChanged();
  void nsfwChanged();
  void lightThemeChanged();
};
