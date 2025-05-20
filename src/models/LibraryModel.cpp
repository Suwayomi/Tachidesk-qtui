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
  if (parent.isValid() || _entries.categories.nodes.empty()) {
    return 0;
  }

  return _entries.categories.nodes[0].mangas.nodes.size();
}

/******************************************************************************
 *
 * Method: data()
 *
 *****************************************************************************/
QVariant LibraryModel::data(const QModelIndex &index, int role) const {
  if (!((index.isValid()) && (index.row() >= 0) &&
        (index.row() < rowCount())) || _entries.categories.nodes.empty()) {
    return {};
  }

  const auto &entry = _entries.categories.nodes[0].mangas.nodes[index.row()];

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
                                         {RoleId, "mangaId"}};

  return roles;
}

/******************************************************************************
 *
 * Method: refreshLibrary()
 *
 *****************************************************************************/
void LibraryModel::refreshLibrary() {
  NetworkManager::instance().postGraphQL(graphql::client::query::AllCategories::GetOperationName(), {},
    [&](graphql::response::Value&& data) {
      auto parsed = graphql::client::query::AllCategories::parseResponse(std::move(data));
      beginResetModel();
      _entries = parsed;
      endResetModel();
    });
  return;
}
