#include "settings.h"

Settings::Settings()
    : _settings("Tachidesk-qtui", "Suwayomi")
{
  load();
  connect(this, &Settings::hostnameChanged, this, &Settings::save);
  connect(this, &Settings::langChanged, this, &Settings::save);
  connect(this, &Settings::nsfwChanged, this, &Settings::save);
  connect(this, &Settings::lightThemeChanged, this, &Settings::save);
  connect(this, &Settings::usernameChanged, this, &Settings::save);
  connect(this, &Settings::passwordChanged, this, &Settings::save);
  connect(this, &Settings::viewerChanged, this, &Settings::save);
  connect(this, &Settings::autoUpdateChanged, this, &Settings::save);
}

Settings::~Settings()
{
  save();
}

void Settings::save()
{
  _settings.setValue("hostname", _hostname);
  _settings.setValue("lang", _lang);
  _settings.setValue("nsfw", _nsfw);
  _settings.setValue("lightTheme", _lightTheme);
  _settings.setValue("username", _username);
  _settings.setValue("password", _password);
  _settings.setValue("viewer", _viewer);
  _settings.setValue("autoUpdate", _autoUpdate);
}

void Settings::load()
{
  _hostname   = _settings.value("hostname", "http://127.0.0.1").toString();
  _lang       = _settings.value("lang", "en").toString();
  _nsfw       = _settings.value("nsfw", false).toBool();
  _lightTheme = _settings.value("lightTheme", false).toBool();
  _viewer     = (Viewer)_settings.value("viewer", Viewer::WEBTOON).toInt();
  _autoUpdate = _settings.value("autoUpdate", false).toBool();
  if (_settings.contains("username")) {
    _username = _settings.value("username").toString();
  }
  if (_settings.contains("password")) {
    _password = _settings.value("password").toString();
  }
}
