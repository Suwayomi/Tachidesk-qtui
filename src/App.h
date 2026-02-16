#pragma once

#include <QSettings>
#include <QTimer>
#include <QQmlApplicationEngine>

#include "commandline.h"
#include "networkmanager.h"
#include "settings.h"
#include "utils/QmlReloader.h"

#include <memory>
#include <QApplication>

#ifdef Q_OS_ANDROID
class QAndroidService;
class QJniObject;
#endif

#ifdef Q_OS_ANDROID
class QAndroidService;
class QJniObject;
#endif
#ifdef Q_OS_ANDROID
#include <QJniObject>
#include <QtCore/private/qandroidextras_p.h>
#endif


class App : public QApplication
{
  Q_OBJECT

  std::shared_ptr<Settings> _settings;

  CommandLine _commandLine;

  NetworkManager _nm;

  // QML engine
  QPointer<QQmlApplicationEngine> _engine;

  std::shared_ptr<QmlReloader> _qmlReloader;

 public:
  App(int argc, char *argv[]);
  ~App();

  Q_INVOKABLE void reload();

 signals:
  void sendMessage(const QString& msg);

 private:
  QUrl makeUrl(const QString& path) const;
  void disconnect();
  void initalize();
};

