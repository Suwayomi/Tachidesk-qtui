//do whatever with this code,
//but qt has some licenses.

#include <QApplication>
#include <QCommandLineParser>
#include <QUrl>
#include <QFontDatabase>

#include "App.h"
#include "commandline.h"

#include <csignal>

int main(int argc, char *argv[])
{
#ifndef _WIN32
  for(auto sig : { SIGINT, SIGHUP, SIGTERM }) {
    std::signal(sig, [](int) { qApp->quit(); });
  }
#endif

  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Round);

#ifndef Q_OS_ANDROID
  QApplication app(argc, argv);
#else
  QGuiApplication app(argc, argv);
#endif

  if (QFontDatabase::addApplicationFont(":/Tachidesk/Qtui/libs/QmlBridgeForMaterialDesignIcons/materialdesignicons-webfont.ttf") < 0) {
    assert(false);
  }

  CommandLine commandline(&app);
  App _app(commandline);

  // Start
  return app.exec();
}
