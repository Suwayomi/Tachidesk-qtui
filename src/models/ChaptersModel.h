#pragma once

#include <QAbstractListModel>
#include <QQmlParserStatus>

#include "../networkmanager.h"
#include "common_structs.h"

class DownloadsModel;

class ChaptersModel : public QAbstractListModel, public QQmlParserStatus
{
  Q_OBJECT

  Q_PROPERTY(NetworkManager* nm READ getNetworkManager WRITE setNetworkManager NOTIFY networkManagerChanged)
  Q_PROPERTY(qint32 mangaNumber     MEMBER _mangaNumber     NOTIFY mangaNumberChanged)
  Q_PROPERTY(qint32 lastReadChapter MEMBER _lastReadChapter NOTIFY lastReadChapterChanged)
  Q_PROPERTY(bool   loading         MEMBER _loading         NOTIFY loadingChanged)
  Q_PROPERTY(bool   autoUpdate      MEMBER _autoUpdate      NOTIFY autoUpdateChanged)

  NetworkManager* _networkManager = nullptr;
  std::shared_ptr<DownloadsModel> _downloads;
  bool _cachedChapters = false;
  bool _loading = true;
  bool _autoUpdate = false;

  qint32 _mangaNumber;
  qint32 _lastReadChapter = 0;

protected:

  void classBegin() override;

  void componentComplete() override;

  virtual QHash<int, QByteArray> roleNames() const override;

private:
  std::vector<ChapterInfo> _chapters;

public:
  enum DownloadOption {
    DownloadAll,
    DownloadUnread,
    DownloadCustom,
  };
  Q_ENUM(DownloadOption)

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
    RoleDownloadProgress,
  };

  ChaptersModel(QObject* parent = nullptr);
  ~ChaptersModel();

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

  Q_INVOKABLE void chapterRead(quint64 chapter, bool read);
  Q_INVOKABLE void previousChaptersRead(qint32 chapter, bool read);
  Q_INVOKABLE void downloadChapter(qint32 downloadOption, qint32 chapterindex = 0);
  Q_INVOKABLE void requestChapters(bool onlineFetch);

signals:
   void networkManagerChanged();
   void mangaNumberChanged();
   void lastReadChapterChanged();
   void loadingChanged();
   void autoUpdateChanged();

public slots:
  void gotChapters(const QJsonDocument& reply);
  void onDownloadsUpdated(const std::vector<QueueInfo>& info);
  void receivePatchReply();
};
