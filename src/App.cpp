#include <QDebug>
#include <QUrl>
#include <tuple>
#include <QDir>
#include <qstringbuilder.h>
#include <QQmlContext>

#include "App.h"

App::App(const CommandLine& cmd, QObject * parent)
  : QObject(parent)
  , _settings(std::make_shared<Settings>())
  , _nm(std::make_unique<NetworkManager>(
          _settings,
          cmd.isSet(CommandLine::hostname)
            ? cmd.value(CommandLine::hostname)
            : _settings->hostname()))
  , _commandLine(cmd)
  , _engine(new QQmlApplicationEngine())
{
  initalize();
}

App::~App() {
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
    return QUrl(QStringLiteral("qrc:/Tachidesk/qtui/src/qml/") % path);
  }
}

void App::reload()
{
  _engine->clearComponentCache();
}

/****************************************************************************
 *
 * Method::initalize()
 *
 ***************************************************************************/
void App::initalize()
{
  // Global context variables to inject into QML
  const std::pair<const char*, QObject*> contextVars[] = {
    { "networkManager", _nm.get()},
    { "settings", _settings.get()},
    { "app", this},
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
