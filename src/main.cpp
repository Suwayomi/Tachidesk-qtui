//do whatever with this code,
//but qt has some licenses.

#include <QApplication>
#include <QCommandLineParser>
#include <QUrl>

#include "ircchat.h"
#include "commandline.h"

#include <csignal>

int main(int argc, char *argv[])
{
  for(auto sig : { SIGINT, SIGHUP, SIGTERM }) {
    std::signal(sig, [](int) { qApp->quit(); });
  }

  QApplication app(argc, argv);

  CommandLine commandline(&app);
  App _app(commandline);

  // Start
  return app.exec();
}
