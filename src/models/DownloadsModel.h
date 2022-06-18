#pragma once

#include <QAbstractListModel>
#include <QQmlParserStatus>
#include <QtWebSockets/QtWebSockets>

#include "ChaptersModel.h"
#include "../networkmanager.h"
#include "MangaDetailsModel.h"
#include "common_structs.h"

class DownloadsModel : public QAbstractListModel, public QQmlParserStatus
{
  Q_OBJECT
  Q_INTERFACES(QQmlParserStatus)

  Q_PROPERTY(NetworkManager* nm READ getNetworkManager WRITE setNetworkManager NOTIFY networkManagerChanged)
  Q_PROPERTY(QString status MEMBER _status NOTIFY statusChanged)

  QString _status;
  NetworkManager* _networkManager = nullptr;

  QWebSocket _webSocket;
  std::vector<QueueInfo> _queue;

  typedef quint32 MangaId;
  QMap<MangaId, MangaDetails> _mangaInfo;

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
    RoleFetchedAt,

    // info
    // ChapterIndex
    RoleMangaId,
    RoleState,
    RoleProgress,
    RoleTries,

    // manga info
    RoleThumbnail,
    RoleTitle,
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

  void setupWebsocket();

  Q_INVOKABLE void clear();
  Q_INVOKABLE void pause();
  Q_INVOKABLE void start();
  Q_INVOKABLE void cancel(qint32 index);
signals:
  void statusChanged();
  void networkManagerChanged();
  void downloadsUpdated(const std::vector<QueueInfo>& info);

public slots:
  void onConnected();
  void closed();
  void onTextMessageReceived(const QString& message);
  void gotDetails(const QJsonDocument&);
};
