#include "LibraryModel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <qcoreapplication.h>
#include <QQmlEngine>

#define exportQmlType(ns, cls) qmlRegisterType<cls>(#ns, 1, 0, #cls)
#define IMPLEMENT_QTQUICK_TYPE(ns, cls) \
  void register ## cls ## Type() { exportQmlType(ns, cls); } \
  Q_COREAPP_STARTUP_FUNCTION(register ## cls ## Type)

IMPLEMENT_QTQUICK_TYPE(Tachidesk.Models, LibraryModel)

/******************************************************************************
 *
 * LibraryModel
 *
 *****************************************************************************/
LibraryModel::LibraryModel(QObject* parent)
  : QAbstractListModel(parent)
{
  connect(
      this,
      &LibraryModel::networkManagerChanged,
      [&]() {
        _networkManager->get("library");
        disconnect(this, &LibraryModel::networkManagerChanged, this, nullptr);

        connect(
            _networkManager,
            &NetworkManager::recievedReply,
            this,
            &LibraryModel::recievedReply);
      });

}

/******************************************************************************
 *
 * Method: recieveReply()
 *
 *****************************************************************************/
void LibraryModel::recievedReply(const QJsonDocument& reply)
{
  //disconnect(_networkManager, &NetworkManager::recievedReply, this, nullptr);
  if (!reply.isArray()) {
    return;
  }

  beginResetModel();
  _entries.clear();


  for (const auto& entry_arr : reply.array()) {
    const auto& entry = entry_arr.toObject();
    auto& info        = _entries.emplace_back();
    info.id           = entry["id"].toInt();
    info.sourceId     = entry["sourceId"].toString();
    info.url          = entry["url"].toString();
    info.title        = entry["title"].toString();
    info.thumbnailUrl = entry["thumbnailUrl"].toString();
    info.initalized   = entry["intialized"].toBool();
    info.author       = entry["author"].toString();
    info.artist       = entry["artist"].toString();
    info.genre        = entry["genre"].toString();
    info.status       = entry["status"].toString();
  }

  endResetModel();
}

/******************************************************************************
 *
 * Method: rowCount()
 *
 *****************************************************************************/
int LibraryModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return 0;
  }

  return _entries.size();
}

/******************************************************************************
 *
 * Method: data()
 *
 *****************************************************************************/
QVariant LibraryModel::data(const QModelIndex &index, int role) const {
  if (!((index.isValid()) &&
       (index.row() >= 0) &&
       (index.row() < rowCount())))
  {
    return {};
  }

  const auto& entry = _entries[index.row()];

  switch (role)
  {
    case RoleTitle:
      {
        return entry.title;
      }
    case RoleThumbnail:
      {
        return QStringLiteral("%1:%2%3").arg(_networkManager->hostname()).arg(_networkManager->port()).arg(entry.thumbnailUrl);
      }
    case RoleId:
      {
        return entry.id;
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
QHash<int, QByteArray> LibraryModel::roleNames() const {
  static QHash<int, QByteArray> roles = { {RoleTitle, "title"},
                                          {RoleThumbnail, "thumbnailUrl"},
                                          {RoleId, "mangaId"} };

  return roles;
}

