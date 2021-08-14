#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <QCommandLineParser>
#include <QCommandLineOption>

class CommandLine
{
public:
  static const QCommandLineOption hostname;
  static const QCommandLineOption port;
  static const QCommandLineOption qmldir;

private:
  QCommandLineParser _parser;

public:
  static CommandLine& instance();

  CommandLine(QCoreApplication* app);
  ~CommandLine();

  bool isSet(
      const QCommandLineOption& option) const;

  QString value(
      const QCommandLineOption& option) const;
};

#endif //COMMANDLINE_H
