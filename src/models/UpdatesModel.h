#pragma once

#include <QAbstractListModel>
#include <QQmlParserStatus>

#include "ChaptersModel.h"
#include "networkmanager.h"

#include <optional>

class UpdatesModel : public QAbstractListModel, public QQmlParserStatus
{
  Q_OBJECT
  Q_INTERFACES(QQmlParserStatus)

  Q_PROPERTY(NetworkManager* nm READ getNetworkManager WRITE setNetworkManager NOTIFY networkManagerChanged)
  Q_PROPERTY(QString source MEMBER _source NOTIFY sourceChanged)

  NetworkManager* _networkManager = nullptr;

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
  };
  std::vector<SourceInfo> _sources;

  QString _source;
  qint32 _pageNumber = 0;

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
  Q_INVOKABLE void downloadChapter(int index);
signals:
  void sourceChanged();
  void networkManagerChanged();
};
