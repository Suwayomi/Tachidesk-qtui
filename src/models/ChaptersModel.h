#pragma once

#include <QAbstractListModel>
#include <QQmlParserStatus>

#include "networkmanager.h"

class ChaptersModel : public QAbstractListModel, public QQmlParserStatus
{
  Q_OBJECT

  Q_PROPERTY(NetworkManager* nm READ getNetworkManager WRITE setNetworkManager NOTIFY networkManagerChanged)
  Q_PROPERTY(qint32 mangaNumber MEMBER _mangaNumber NOTIFY networkManagerChanged)

  NetworkManager* _networkManager = nullptr;
  bool _cachedChapters = false;

  struct ChapterInfo {
    QString  url;
    QString  name;
    qint32   chapterNumber;
    bool     read;
    bool     downloaded;
    quint32  index;
    quint32  pageCount;
    quint32  chapterCount;
    quint32  lastPageRead;
  };
  std::vector<ChapterInfo> _chapters;

  qint32 _mangaNumber;

  void requestChapters(bool onlineFetch);
protected:

  void classBegin() override;

  void componentComplete() override;

  virtual QHash<int, QByteArray> roleNames() const override;

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

  Q_INVOKABLE void chapterRead(qint32 chapter);
  Q_INVOKABLE void downloadChapter(qint32 downloadOption);

signals:
   void networkManagerChanged();

public slots:
  void gotChapters(const QJsonDocument& reply);
};
