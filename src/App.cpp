#include <QDebug>
#include <QUrl>
#include <tuple>
#include <QDir>
#include <qstringbuilder.h>
#include <QQmlContext>
#include <QAuthenticator>
#include <QFontDatabase>

#include "App.h"
#include "networkimageprovider.h"

App::App(int argc, char *argv[])
  : QApplication(argc, argv)
  , _settings(std::make_shared<Settings>())
  , _commandLine(this)
  , _nm(_settings,
          _commandLine.isSet(CommandLine::hostname)
            ? _commandLine.value(CommandLine::hostname)
            : _settings->hostname(),
          this)
  , _engine(new QQmlApplicationEngine(this))
  , _qmlReloader(std::make_shared<QmlReloader>(_engine, this))
{
  initalize();
}

App::~App() {
  qDebug() << "App destructor called";
  disconnect();
  if (_engine) {
    delete _engine;
    _engine = nullptr;
  }
  _qmlReloader.reset();
  _settings.reset();
  qDebug() << "App destructor finished";
}

void App::disconnect() {
  _nm.deleteLater();
}

QUrl App::makeUrl(const QString& path) const
{
  if (auto qmldir = _commandLine.value(CommandLine::qmldir);
      !qmldir.isEmpty())
  {
    return QUrl::fromLocalFile(QDir(qmldir).absoluteFilePath(path));
  }
  else {
    return QUrl(QStringLiteral("qrc:/Tachidesk/Qtui/src/qml/") % path);
  }
}

void App::reload()
{
  _engine->clearComponentCache();
}

void App::initalize()
{
  if (QFontDatabase::addApplicationFont(":/Tachidesk/Qtui/libs/QmlBridgeForMaterialDesignIcons/materialdesignicons-webfont.ttf") < 0) {
    assert(false);
  }

  connect(this, &QApplication::applicationStateChanged,
    [&](Qt::ApplicationState state)
    {
      qDebug() << "Application state changed to" << state;
      if (state == Qt::ApplicationActive) {
        _engine->load(QUrl(u"qrc:/main.qml"_qs));
        qDebug() << "Reloaded QML";
      }
    }
  );

  const std::pair<const char*, QObject*> contextVars[] = {
    { "networkManager", &_nm},
    { "settings", _settings.get()},
    { "qmlReloader", _qmlReloader.get()},
    { "app", this },
  };
  auto context = _engine->rootContext();
  for (auto& var : contextVars) {
    context->setContextProperty(var.first, QVariant::fromValue(var.second));
  }

  _engine->setNetworkAccessManagerFactory(&_nm);
  _engine->addImportPath(QStringLiteral("qrc:/"));
  _engine->load(makeUrl(QStringLiteral("main.qml")));
  auto *imageProvider = new NetworkImageProvider(_nm.username(), _nm.password());
  _engine->addImageProvider(QLatin1String("network"), imageProvider);

  connect(_engine, &QQmlApplicationEngine::quit, this, &QCoreApplication::quit);

   connect(this, &QCoreApplication::aboutToQuit, []() {
     qDebug() << "Application is about to quit!";
   });
 }