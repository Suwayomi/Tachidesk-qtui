#include "ExtensionModel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <qcoreapplication.h>
#include <QQmlEngine>

#define exportQmlType(ns, cls) qmlRegisterType<cls>(#ns, 1, 0, #cls)
#define IMPLEMENT_QTQUICK_TYPE(ns, cls) \
  void register ## cls ## Type() { exportQmlType(ns, cls); } \
  Q_COREAPP_STARTUP_FUNCTION(register ## cls ## Type)

IMPLEMENT_QTQUICK_TYPE(Tachidesk.Models, ExtensionModel)

/******************************************************************************
 *
 * ExtensionModel
 *
 *****************************************************************************/
ExtensionModel::ExtensionModel(QObject* parent)
  : QAbstractListModel(parent)
{
}

void ExtensionModel::classBegin() { }

/******************************************************************************
 *
 * componentComplete
 *
 *****************************************************************************/
void ExtensionModel::componentComplete()
{
  _networkManager->getExtensions("extension/list");

  connect(
      _networkManager,
      &NetworkManager::receiveExtensions,
      this,
      &ExtensionModel::receivedReply);
}

/******************************************************************************
 *
 * Method: receiveReply()
 *
 *****************************************************************************/
void ExtensionModel::receivedReply(const QJsonDocument& reply)
{
  beginResetModel();
  _extensions.clear();

  uint32_t i = 0;
  for (const auto& entry_arr : reply.array()) {
    const auto& entry = entry_arr.toObject();
    auto& info        = _extensions.emplace_back();
    info.iconUrl      = entry["iconUrl"].toString();
    info.name         = entry["name"].toString();
    info.pkgName      = entry["pkgName"].toString();
    info.lang         = entry["lang"].toString();
    info.isNsfw       = entry["isNsfw"].toBool();
    info.installed    = entry["installed"].toBool();
    info.hasUpdate    = entry["hasUpdate"].toBool();
    info.obsolete     = entry["obsolete"].toBool();
    info.index        = i;
    ++i;
  }

  endResetModel();
}

/******************************************************************************
 *
 * Method: rowCount()
 *
 *****************************************************************************/
int ExtensionModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return 0;
  }

  return _extensions.size();
}

/******************************************************************************
 *
 * Method: data()
 *
 *****************************************************************************/
QVariant ExtensionModel::data(const QModelIndex &index, int role) const {
  if (!((index.isValid()) &&
       (index.row() >= 0) &&
       (index.row() < rowCount())))
  {
    return {};
  }

  const auto& entry = _extensions[index.row()];

  switch (role)
  {
    case RoleIconUrl:
      {
        return _networkManager->resolvedPath().arg(entry.iconUrl);
      }
    case RoleName:
      {
        return entry.name;
      }
    case RolePkgName:
      {
        return entry.pkgName;
      }
    case RoleLang:
      {
        return entry.lang;
      }
    case RoleNsfw:
      {
        return entry.isNsfw;
      }
    case RoleInstalled:
      {
        return entry.installed;
      }
    case RoleUpdate:
      {
        return entry.hasUpdate;
      }
    case RoleObsolete:
      {
        return entry.obsolete;
      }
    case RoleRow:
      {
        return entry.index;
      }
    //case Role
    default:
      return {};
  }

  return {};
}

/******************************************************************************
 *
 * Method: roleNames()
 *
 *****************************************************************************/
QHash<int, QByteArray> ExtensionModel::roleNames() const {
  static QHash<int, QByteArray> roles = { {RoleIconUrl,       "iconUrl"},
                                          {RoleName,          "name"},
                                          {RolePkgName,       "pkgName"},
                                          {RoleLang,          "lang"},
                                          {RoleNsfw,          "isNsfw"},
                                          {RoleInstalled,     "installed"},
                                          {RoleUpdate,        "hasUpdate"},
                                          {RoleRow,           "row"},
                                          {RoleObsolete,      "obsolete"},};

  return roles;
}

void ExtensionModel::update(const QString& pkgName, qint32 row)
{
  _extensions[row].hasUpdate = false;
  _networkManager->get("extension/update/" + pkgName);

  emit dataChanged(index(row, 0), index(row, 0), {RoleUpdate});
}

void ExtensionModel::install(const QString& pkgName, qint32 row)
{
  _extensions[row].installed = true;
  _networkManager->get("extension/install/" + pkgName);

  emit dataChanged(index(row, 0), index(row, 0), {RoleInstalled});
}

void ExtensionModel::uninstall(const QString& pkgName, qint32 row)
{
  _extensions[row].installed = false;
  _networkManager->get("extension/uninstall/" + pkgName);

  emit dataChanged(index(row, 0), index(row, 0), {RoleInstalled});
}
