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
}

void LibraryModel::classBegin()
{
  gotCategory = [&](const auto& reply) {
    for (const auto& entry_arr : reply.array()) {
      const auto& entry = entry_arr.toObject();
      _networkManager->get(QStringLiteral("category/%1").arg(entry["id"].toInt()), gotCategoryId);
    }
  };

  /******************************************************************************
   *
   * Method: receiveReply()
   *
   *****************************************************************************/
  gotCategoryId = [&](const QJsonDocument& reply)
  {
    beginResetModel();

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
      info.unread       = entry["unreadCount"].toInt();
    }

    endResetModel();
  };
}

/******************************************************************************
 *
 * componentComplete
 *
 *****************************************************************************/
void LibraryModel::componentComplete()
{
  _networkManager->get("category", gotCategory);
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
        return _networkManager->resolvedPath().arg(entry.thumbnailUrl);
      }
    case RoleId:
      {
        return entry.id;
      }
    case RoleUnread:
      {
        return entry.unread;
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
                                          {RoleUnread, "unread"},
                                          {RoleId, "mangaId"} };

  return roles;
}

/******************************************************************************
 *
 * Method: refreshLibrary()
 *
 *****************************************************************************/
void LibraryModel::refreshLibrary()
{
  _entries.clear();
  _networkManager->get("category", gotCategory);
}

