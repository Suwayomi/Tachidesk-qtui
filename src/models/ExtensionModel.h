#pragma once

#include <QAbstractListModel>
#include <QQmlParserStatus>
#include <qqml.h>

class ExtensionModel : public QAbstractListModel, public QQmlParserStatus
{
  Q_OBJECT
  QML_ELEMENT
  Q_INTERFACES(QQmlParserStatus)

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

  Q_INVOKABLE void update(const QString& pkgName, qint32 index);
  Q_INVOKABLE void install(const QString& pkgName, qint32 index);
  Q_INVOKABLE void uninstall(const QString& pkgName, qint32 index);
};
