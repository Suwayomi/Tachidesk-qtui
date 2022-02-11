#pragma once

#include <QAbstractListModel>
#include <QQmlParserStatus>

#include "../networkmanager.h"

class ExtensionModel : public QAbstractListModel, public QQmlParserStatus
{
  Q_OBJECT
  Q_INTERFACES(QQmlParserStatus)

  Q_PROPERTY(NetworkManager* nm READ getNetworkManager WRITE setNetworkManager NOTIFY networkManagerChanged)

  NetworkManager* _networkManager = nullptr;

  struct ExtensionInfo {
    QString  iconUrl;
    QString  pkgName;
    QString  name;
    QString  lang;
    bool     isNsfw;
    bool     installed;
    bool     hasUpdate;
    bool     obsolete;
    uint32_t index;
  };
  std::vector<ExtensionInfo> _extensions;

protected:

  void classBegin() override;

  void componentComplete() override;

  virtual QHash<int, QByteArray> roleNames() const override;

public:

  enum Role {
    RoleIconUrl = Qt::UserRole + 1,
    RoleName,
    RolePkgName,
    RoleLang,
    RoleNsfw,
    RoleInstalled,
    RoleUpdate,
    RoleObsolete,
    RoleRow,
  };

  ExtensionModel(QObject* parent = nullptr);

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

  Q_INVOKABLE void update(const QString& pkgName, qint32 index);
  Q_INVOKABLE void install(const QString& pkgName, qint32 index);
  Q_INVOKABLE void uninstall(const QString& pkgName, qint32 index);

signals:
   void networkManagerChanged();
};
