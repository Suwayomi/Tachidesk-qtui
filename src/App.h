#ifndef IRCCHAT_H
#define IRCCHAT_H

#include <QSettings>
#include <QTimer>
#include <QQmlApplicationEngine>

#include "commandline.h"
#include "networkmanager.h"
#include "settings.h"

#include <memory>


class App : public QObject
{
  Q_OBJECT

  std::shared_ptr<Settings> _settings;

  std::unique_ptr<NetworkManager> _nm;

  const CommandLine& _commandLine;

  // QML engine
  QPointer<QQmlApplicationEngine> _engine;

public:
  App(const CommandLine& cmd, QObject * parent = 0);
  ~App();

  Q_INVOKABLE void reload();
signals:
  void sendMessage(const QString& msg);

private:
  QUrl makeUrl(const QString& path) const;
  void disconnect();
  void initalize();
};

#endif //IRCCHAT_H
