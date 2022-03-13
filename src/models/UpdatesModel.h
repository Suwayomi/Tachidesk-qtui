#pragma once

#include <QAbstractListModel>
#include <QQmlParserStatus>
#include <QWebSocket>

#include "ChaptersModel.h"
#include "../networkmanager.h"

#include <optional>

class DownloadsModel;
class QueueInfo;

class UpdatesModel : public QAbstractListModel, public QQmlParserStatus
{
  Q_OBJECT
  Q_INTERFACES(QQmlParserStatus)

  Q_PROPERTY(NetworkManager* nm READ getNetworkManager WRITE setNetworkManager NOTIFY networkManagerChanged)
  Q_PROPERTY(bool running MEMBER _running NOTIFY runningChanged)
  Q_PROPERTY(qint32 total MEMBER _total NOTIFY totalChanged)
  Q_PROPERTY(qint32 complete MEMBER _complete NOTIFY completeChanged)

  NetworkManager* _networkManager = nullptr;
  QWebSocket _webSocket;
  std::shared_ptr<DownloadsModel> downloads;

  struct SourceInfo {
    qint32   id;
    QString  sourceId;
    QString  title;
    QString  url;
    QString  thumbnailUrl;
    bool     isInitialized;
    bool     inLibrary;
    bool     freshData;

    ChapterInfo chapterInfo;
    std::shared_ptr<QueueInfo> queueInfo;
  };
  std::vector<SourceInfo> _sources;

  bool _running = false;
  qint32 _total = 0;
  qint32 _complete = 0;
  qint32 _pageNumber = 0;
  bool _hasNext = false;

protected:

  void classBegin() override;

  void componentComplete() override;

  virtual QHash<int, QByteArray> roleNames() const override;

public:

  enum Role {
    RoleThumbnailUrl = Qt::UserRole + 1,
    RoleTitle,
    RoleId,
    RoleUrl,
    RoleInitialized,
    RoleInLibrary,
    RoleFreshData,
    RoleChapterUrl,
    RoleName,
    RoleChapterNumber,
    RoleRead,
    RoleChapterIndex,
    RolePageCount,
    RoleChapterCount,
    RoleLastPageRead,
    RoleDownloaded,
    RoleFetchedAt,
    RoleDownloadProgress,
  };

  UpdatesModel(QObject* parent = nullptr);

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

  void receivedReply(const QJsonDocument& reply);

  Q_INVOKABLE void next();
  Q_INVOKABLE void refresh();
  Q_INVOKABLE void downloadChapter(int index);
  Q_INVOKABLE void chapterRead(qint32 mangaId, qint32 chapter);
signals:
  void networkManagerChanged();
  void runningChanged();
  void totalChanged();
  void completeChanged();

public slots:
  void onConnected();
  void closed();
  void onTextMessageReceived(const QString& message);
  void onDownloadsUpdated(const std::vector<QueueInfo>& info);
};
