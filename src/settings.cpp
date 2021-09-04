#include "settings.h"

Settings::Settings()
    : _settings("Tachidesk-qtui", "Suwayomi")
{
  load();
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
}

void Settings::load()
{
  _hostname = _settings.value("hostname", "http://127.0.0.1").toString();
  _lang     = _settings.value("lang", "en").toString();
  _nsfw     = _settings.value("nsfw", false).toBool();
}
