#pragma once

#include <QAbstractListModel>
#include <QQmlParserStatus>

#include "networkmanager.h"

class MangaDetailsModel : public QAbstractListModel, public QQmlParserStatus
{
  Q_OBJECT

  Q_PROPERTY(NetworkManager* nm READ getNetworkManager WRITE setNetworkManager NOTIFY networkManagerChanged)
  Q_PROPERTY(qint32 mangaNumber MEMBER _mangaNumber NOTIFY mangaNumberChanged)

  NetworkManager* _networkManager = nullptr;

  struct EntryInfo {
    qint32   id;
    QString  sourceId;
    QString  url;
    QString  title;
    QString  thumbnailUrl;
    bool     initalized = false;
    QString  artist;
    QString  author;
    QString  description;
    QString  genre;
    QString  status;
    bool     inLibrary;
    // supportsLatest
    // isConfigurable
  };
  std::vector<EntryInfo> _entries;

  qint32 _mangaNumber;

  std::function<void (const QJsonDocument& )> gotDetails;
protected:

  void classBegin() override;

  void componentComplete() override;

  virtual QHash<int, QByteArray> roleNames() const override;

public:

  enum Role {
    RoleId = Qt::UserRole + 1,
    RoleSourceId,
    RoleUrl,
    RoleTitle,
    RoleThumbnailUrl,
    RoleInitialized,
    RoleAuthor,
    RoleArtist,
    RoleDescription,
    RoleGenre,
    RoleStatus,
    RoleInLibrary,
  };

  MangaDetailsModel(QObject* parent = nullptr);

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

  Q_INVOKABLE QVariantMap get(int row) const;

  Q_INVOKABLE void addToLibrary();
  Q_INVOKABLE void removeFromLibrary();
signals:
   void networkManagerChanged();
   void mangaNumberChanged();
   void loaded();
};