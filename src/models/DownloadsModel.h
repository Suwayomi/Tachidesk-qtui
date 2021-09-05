#pragma once

#include <QAbstractListModel>
#include <QQmlParserStatus>
#include <QtWebSockets/QtWebSockets>

#include "ChaptersModel.h"
#include "networkmanager.h"

class DownloadsModel : public QAbstractListModel, public QQmlParserStatus
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
    ChaptersModel::ChapterInfo chapterInfo;
  };
  std::vector<QueueInfo> _queue;

protected:

  void classBegin() override;

  void componentComplete() override;

  virtual QHash<int, QByteArray> roleNames() const override;

public:

  enum Role {
    RoleUrl = Qt::UserRole + 1,
    RoleName,
    RoleChapterNumber,
    RoleRead,
    RoleChapterIndex,
    RolePageCount,
    RoleChapterCount,
    RoleLastPageRead,
    RoleDownloaded,

    // info
    // ChapterIndex
    RoleMangaId,
    RoleState,
    RoleProgress,
    RoleTries,
  };

  DownloadsModel(QObject* parent = nullptr);

  virtual int rowCount(
     const QModelIndex &parent = QModelIndex()) const override;

  virtual QVariant data(
     const QModelIndex &index,
     int role = Qt::DisplayRole) const override;

  auto getNetworkManager() const
  {
    return _networkManager;
  }

  void setNetworkManager(NetworkManager* nm) {
    _networkManager = nm;
    networkManagerChanged();
  }

  Q_INVOKABLE void clear();
signals:
  void statusChanged();
  void networkManagerChanged();

public slots:
  void onConnected();
  void closed();
  void onTextMessageReceived(const QString& message);
};
