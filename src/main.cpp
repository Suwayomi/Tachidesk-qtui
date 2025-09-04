//do whatever with this code,
//but qt has some licenses.

#include <QCommandLineParser>
#include <QUrl>

#include "App.h"

#include <csignal>
#include <qloggingcategory.h>

int main(int argc, char *argv[])
{
#ifndef _WIN32
  for(auto sig : { SIGINT, SIGHUP, SIGTERM }) {
    std::signal(sig, [](int) { qApp->quit(); });
  }
#endif

  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Round);

  QLoggingCategory::setFilterRules(
    "qt.network.ssl=true\n"      // if you don’t care about SSL noise
    "qt.network.http2=false\n"     // or false, to see HTTP/2 negotiation
    "qt.network.auth=true\n"
    "qt.network.accessmanager=true\n"
    "qt.diskcache=true"
);

  App app(argc, argv);

  // Start
  return app.exec();
}
