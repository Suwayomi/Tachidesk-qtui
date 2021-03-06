#include "SourcesModel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <qcoreapplication.h>
#include <QQmlEngine>

#define exportQmlType(ns, cls) qmlRegisterType<cls>(#ns, 1, 0, #cls)
#define IMPLEMENT_QTQUICK_TYPE(ns, cls) \
  void register ## cls ## Type() { exportQmlType(ns, cls); } \
  Q_COREAPP_STARTUP_FUNCTION(register ## cls ## Type)

IMPLEMENT_QTQUICK_TYPE(Tachidesk.Models, SourcesModel)

/******************************************************************************
 *
 * SourcesModel
 *
 *****************************************************************************/
SourcesModel::SourcesModel(QObject* parent)
  : QAbstractListModel(parent)
{
}

void SourcesModel::classBegin() { }

/******************************************************************************
 *
 * componentComplete
 *
 *****************************************************************************/
void SourcesModel::componentComplete()
{
  _networkManager->get("source/list");

  connect(
      _networkManager,
      &NetworkManager::receivedReply,
      this,
      &SourcesModel::receivedReply);
}

/******************************************************************************
 *
 * Method: receiveReply()
 *
 *****************************************************************************/
void SourcesModel::receivedReply(const QJsonDocument& reply)
{
  disconnect(_networkManager, &NetworkManager::receivedReply, this, nullptr);

  beginResetModel();
  _sources.clear();

  for (const auto& entry_arr : reply.array()) {
    const auto& entry   = entry_arr.toObject();
    auto& info          = _sources.emplace_back();
    info.iconUrl        = entry["iconUrl"].toString();
    info.name           = entry["name"].toString();
    info.lang           = entry["lang"].toString();
    info.id             = entry["id"].toString();
    info.supportsLatest = entry["supportsLatest"].toBool();
    info.isConfigurable = entry["isConfigurable"].toBool();
  }

  endResetModel();
}

/******************************************************************************
 *
 * Method: rowCount()
 *
 *****************************************************************************/
int SourcesModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return 0;
  }

  return _sources.size();
}

/******************************************************************************
 *
 * Method: data()
 *
 *****************************************************************************/
QVariant SourcesModel::data(const QModelIndex &index, int role) const {
  if (!((index.isValid()) &&
       (index.row() >= 0) &&
       (index.row() < rowCount())))
  {
    return {};
  }

  const auto& entry = _sources[index.row()];

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
    case RoleLang:
      {
        return entry.lang;
      }
    case RoleId:
      {
        return entry.id;
      }
    case RoleSupportsLatest:
      {
        return entry.supportsLatest;
      }
    case RoleConfigurable:
      {
        return entry.isConfigurable;
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
QHash<int, QByteArray> SourcesModel::roleNames() const {
  static QHash<int, QByteArray> roles = { {RoleIconUrl,       "iconUrl"},
                                          {RoleName,          "name"},
                                          {RoleId,            "id"},
                                          {RoleLang,          "lang"},
                                          {RoleSupportsLatest,"supportsLatest"},
                                          {RoleConfigurable,  "isConfigurable"},};

  return roles;
}
