#pragma once

#include <QAbstractListModel>
#include <QQmlParserStatus>

#include "networkmanager.h"

#include <optional>

class SourcesLibraryModel : public QAbstractListModel, public QQmlParserStatus
{
  Q_OBJECT

  Q_PROPERTY(NetworkManager* nm READ getNetworkManager WRITE setNetworkManager NOTIFY networkManagerChanged)
  Q_PROPERTY(QString source MEMBER _source NOTIFY sourceChanged)

  NetworkManager* _networkManager = nullptr;

  struct SourceDetails {
    QString  artist;
    QString  author;
    QString  description;
    QString  genre;
    QString  status;
  };

  struct SourceInfo {
    qint32   id;
    QString  title;
    QString  thumbnailUrl;
    QString  url;
    bool     isInitialized;
    bool     inLibrary;
    bool     freshData;

    std::optional<SourceDetails> details;
  };
  std::vector<SourceInfo> _sources;

  QString _source;
  qint32 _pageNumber = 1;

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
  };

  SourcesLibraryModel(QObject* parent = nullptr);

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

  Q_INVOKABLE void search(const QString& searchTerm);
signals:
  void sourceChanged();
  void networkManagerChanged();
};
