#pragma once
#include <QAbstractListModel>

#include "networkmanager.h"

class LibraryModel : public QAbstractListModel
{
  Q_OBJECT

  Q_PROPERTY(NetworkManager* nm READ getNetworkManager WRITE setNetworkManager NOTIFY networkManagerChanged)

  NetworkManager* _networkManager = nullptr;

  struct EntryInfo {
    uint32_t id;
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
  };
  std::vector<EntryInfo> _entries;

protected:
  virtual QHash<int, QByteArray> roleNames() const override;

public:

  enum Role {
    RoleTitle = Qt::UserRole + 1,
    RoleThumbnail,
    RoleId,
  };


  LibraryModel(QObject* parent = nullptr);

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

  void recievedReply(const QJsonDocument& reply);

signals:
   void networkManagerChanged();
};
