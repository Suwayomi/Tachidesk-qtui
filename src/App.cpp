#include <QDebug>
#include <QUrl>
#include <QRegExp>
#include <tuple>
#include <QDir>
#include <qstringbuilder.h>
#include <QQmlContext>

#include "App.h"

App::App(const CommandLine& cmd, QObject * parent)
  : QObject(parent)
  , _settings("Tachidesk-qtui", "Suwayomi")
  , _nm(std::make_unique<NetworkManager>(cmd.value(CommandLine::hostname), cmd.value(CommandLine::port)))
  , _commandLine(cmd)
  , _engine(new QQmlApplicationEngine())
{
  initalize();
}

App::~App() {
  saveSettings();
}

void App::saveSettings()
{
  // TODO settings
}

/****************************************************************************
 *
 * Method: makeUrl()
 *
 ***************************************************************************/
QUrl App::makeUrl(const QString& path) const
{
  if (auto qmldir = _commandLine.value(CommandLine::qmldir);
      !qmldir.isEmpty())
  {
    return QUrl::fromLocalFile(QDir(qmldir).absoluteFilePath(path));
  }
  else {
    return QUrl(QStringLiteral("qrc:/") % path);
  }
}


void App::initalize()
{
  // Global context variables to inject into QML
  const std::pair<const char*, QObject*> contextVars[] = {
    { "networkManager", _nm.get()},
  };
  auto context = _engine->rootContext();
  for (auto& var : contextVars) {
    context->setContextProperty(var.first, QVariant::fromValue(var.second));
  }

  _engine->addImportPath(QStringLiteral("qrc:/"));
  _engine->load(makeUrl(QStringLiteral("main.qml")));
}

void App::disconnect() {
}
