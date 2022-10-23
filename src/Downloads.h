#pragma once

#include <QAbstractListModel>
#include <QQmlParserStatus>
#include <QtWebSockets/QtWebSockets>

#include "models/ChaptersModel.h"

class Downloads : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString status MEMBER _status NOTIFY statusChanged)

  QString _status;

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

  Q_INVOKABLE void clear();
  Q_INVOKABLE void cancel(qint32 index);

signals:
  void statusChanged();
  void queueUpdated();

public slots:
  void onConnected();
  void closed();
  void onTextMessageReceived(const QString& message);
};
