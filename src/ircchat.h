#ifndef IRCCHAT_H
#define IRCCHAT_H

#include <QSettings>
#include <QTimer>
#include <QQmlApplicationEngine>

#include "commandline.h"
#include "networkmanager.h"

#include <memory>


class App : public QObject
{
  Q_OBJECT

  QSettings _settings;

  std::unique_ptr<NetworkManager> _nm;

  const CommandLine& _commandLine;

  // QML engine
  QPointer<QQmlApplicationEngine> _engine;

public:
  App(const CommandLine& cmd, QObject * parent = 0);
  ~App();

signals:
  void sendMessage(const QString& msg);

private:
  QUrl makeUrl(const QString& path) const;
  void disconnect();
  void initalize();
  void saveSettings();
};

#endif //IRCCHAT_H
