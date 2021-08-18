#include "commandline.h"

const QCommandLineOption CommandLine::hostname(
    "hostname", "server's hostname", "hostname", "http://127.0.0.1");

const QCommandLineOption CommandLine::port(
    "port", "server's port", "port", "4567");

const QCommandLineOption CommandLine::qmldir(
  "qmldir", "Load qml from filesystem path instead of qrc", "path");


static CommandLine* commandline = nullptr;

CommandLine::CommandLine(QCoreApplication* app)
{
  _parser.addOption(hostname);
  _parser.addOption(port);
  _parser.addOption(qmldir);

  _parser.process(*app);

  _parser.addHelpOption();
  commandline = this;
}

CommandLine::~CommandLine()
{
  commandline = nullptr;
}

CommandLine& CommandLine::instance()
{
  return *commandline;
}

bool CommandLine::isSet(const QCommandLineOption& option) const
{
  return _parser.isSet(option);
}

QString CommandLine::value(const QCommandLineOption& option) const
{
  return _parser.value(option);
}
