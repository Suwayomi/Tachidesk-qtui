#pragma once
#include <QAbstractListModel>
#include <QQmlParserStatus>

#include "networkmanager.h"

class LibraryModel : public QAbstractListModel, public QQmlParserStatus
{
  Q_OBJECT
  Q_INTERFACES(QQmlParserStatus)

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

  std::function<void (const QJsonDocument& )> gotCategory;
  std::function<void (const QJsonDocument& )> gotCategoryId;

protected:
  virtual QHash<int, QByteArray> roleNames() const override;

  void classBegin() override;

  void componentComplete() override;

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

  Q_INVOKABLE void refreshLibrary();

signals:
   void networkManagerChanged();

public slots:
};
