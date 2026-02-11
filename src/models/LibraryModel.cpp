#include "LibraryModel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQmlEngine>
#include <QStringBuilder>
#include <qcoreapplication.h>

#include "../networkmanager.h"
#include "../graphql/tachideskClient.h"
#include <graphqlservice/GraphQLClient.h>

/******************************************************************************
 *
 * LibraryModel
 *
 *****************************************************************************/
LibraryModel::LibraryModel(QObject *parent) : QAbstractListModel(parent) {}

/******************************************************************************
 *
 * classBegin
 *
 *****************************************************************************/
void LibraryModel::classBegin() { refreshLibrary(); }

/******************************************************************************
 *
 * componentComplete
 *
 *****************************************************************************/
void LibraryModel::componentComplete() {}

/******************************************************************************
 *
 * Method: rowCount()
 *
 *****************************************************************************/
int LibraryModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return 0;
  }

  return _entries.category.mangas.nodes.size();
}

/******************************************************************************
 *
 * Method: data()
 *
 *****************************************************************************/
QVariant LibraryModel::data(const QModelIndex &index, int role) const {
  if (!((index.isValid()) && (index.row() >= 0) &&
        (index.row() < rowCount()))) {
    return {};
  }

  const auto &entry = _entries.category.mangas.nodes[index.row()];

  switch (role) {
  case RoleTitle: {
    return entry.title.c_str();
  }
  case RoleThumbnail: {
    return NetworkManager::instance().resolvedPath().resolved(
        QString::fromStdString(entry.thumbnailUrl.value_or(""))); //.mid(1));
  }
  case RoleId: {
    return entry.id;
  }
  case RoleUnread: {
    return entry.unreadCount;
  }
  case RoleRecentlyRead: {
    if (!entry.latestReadChapter) {
      return 0;
    }
    return QString::fromStdString(entry.latestReadChapter->lastReadAt.get<std::string>()).toInt();
  }

  // case Role
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
  static QHash<int, QByteArray> roles = {{RoleTitle, "title"},
                                         {RoleThumbnail, "thumbnailUrl"},
                                         {RoleUnread, "unread"},
                                         {RoleId, "mangaId"},
                                         {RoleRecentlyRead, "recentlyRead"},
  };

  return roles;
}

/******************************************************************************
 *
 * Method: refreshLibrary()
 *
 *****************************************************************************/
void LibraryModel::refreshLibrary() {
  QJsonObject variablesObj;
  variablesObj.insert("id", 0);

  NetworkManager::instance().postGraphQL(graphql::qtui::client::query::GET_CATEGORY_MANGAS::GetOperationName(), std::move(variablesObj),
    [&](graphql::response::Value&& data) {
      auto parsed = graphql::qtui::client::query::GET_CATEGORY_MANGAS::parseResponse(std::move(data));
      beginResetModel();
      _entries = std::move(parsed);
      endResetModel();
    });
  return;
}
