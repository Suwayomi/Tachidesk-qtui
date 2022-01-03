#pragma once

#include <QAbstractListModel>
#include <QQmlParserStatus>
#include <QtWebSockets/QtWebSockets>

#include "ChaptersModel.h"
#include "networkmanager.h"

class Downloads : public QObject
{
  Q_OBJECT

  Q_PROPERTY(NetworkManager* nm READ getNetworkManager WRITE setNetworkManager NOTIFY networkManagerChanged)
  Q_PROPERTY(QString status MEMBER _status NOTIFY statusChanged)

  QString _status;
  NetworkManager* _networkManager = nullptr;

  QWebSocket _webSocket;

  struct QueueInfo {
    quint32  chapterIndex;
    quint32  mangaId;
    QString  state;
    quint32  progress;
    quint32  tries;
    ChapterInfo chapterInfo;
  };
  std::vector<QueueInfo> _queue;

public:

  Downloads(QObject* parent = nullptr);

  auto getNetworkManager() const
  {
    return _networkManager;
  }

  void setNetworkManager(NetworkManager* nm) {
    _networkManager = nm;
    networkManagerChanged();
  }

  Q_INVOKABLE void clear();
  Q_INVOKABLE void cancel(qint32 index);

signals:
  void statusChanged();
  void networkManagerChanged();
  void queueUpdated();

public slots:
  void onConnected();
  void closed();
  void onTextMessageReceived(const QString& message);
};
