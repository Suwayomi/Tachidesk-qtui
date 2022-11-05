#include "LibraryModel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <qcoreapplication.h>
#include <QQmlEngine>
#include <QStringBuilder>

#include "../networkmanager.h"
/******************************************************************************
 *
 * LibraryModel
 *
 *****************************************************************************/
LibraryModel::LibraryModel(QObject* parent)
  : QAbstractListModel(parent)
{
}

/******************************************************************************
 *
 * classBegin
 *
 *****************************************************************************/
void LibraryModel::classBegin()
{
  refreshLibrary();
}

/******************************************************************************
 *
 * componentComplete
 *
 *****************************************************************************/
void LibraryModel::componentComplete()
{
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
        return NetworkManager::instance().resolvedPath().resolved(entry.thumbnailUrl.mid(1));
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
  NetworkManager::instance().get(QUrl("category"), this,
    [&](const auto& doc)
  {
    for (const auto& entry_arr : doc.array()) {
      const auto& entry = entry_arr.toObject();
      NetworkManager::instance().get(QUrl(u"category/"_qs % QString::number(entry["id"].toInt())), this,
        [&](const auto& doc1)
      {
        beginResetModel();

        for (const auto& entry_arr : doc1.array()) {
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
      });
    }
  });
}

