#include "SourcesLibraryModel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <qcoreapplication.h>
#include <QQmlEngine>
#include <QStringBuilder>

#include "../networkmanager.h"

/******************************************************************************
 *
 * SourcesLibraryModel
 *
 *****************************************************************************/
SourcesLibraryModel::SourcesLibraryModel(QObject* parent)
  : QAbstractListModel(parent)
{
  receiveReply = [&](const QJsonDocument& reply) {
    const auto& mangaList = reply["mangaList"].toArray();

    if (_resetModel) {
      beginResetModel();
      _sources.clear();
    } else {
      beginInsertRows({}, _sources.size(), _sources.size() + mangaList.count() - 1);
    }

    _hasNextPage = reply["hasNextPage"].toBool();
    qDebug() << "has next page?" << _hasNextPage;

    for (const auto& entry_arr : mangaList) {
      const auto& entry   = entry_arr.toObject();
      auto& info          = _sources.emplace_back();
      info.id             = entry["id"].toInt();
      info.title          = entry["title"].toString();
      info.thumbnailUrl   = entry["thumbnailUrl"].toString();
      info.url            = entry["url"].toString();
      info.isInitialized  = entry["isInitialized"].toBool();
      info.inLibrary      = entry["inLibrary"].toBool();
      if (info.isInitialized) {
        // TODO: fill in details
      }
    }

    if (_resetModel) {
      _resetModel = false;
      endResetModel();
    } else {
      endInsertRows();
    }
  };
}

void SourcesLibraryModel::classBegin() { }

/******************************************************************************
 *
 * componentComplete
 *
 *****************************************************************************/
void SourcesLibraryModel::componentComplete()
{
  next();
}

/******************************************************************************
 *
 * Method: rowCount()
 *
 *****************************************************************************/
int SourcesLibraryModel::rowCount(const QModelIndex &parent) const {
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
QVariant SourcesLibraryModel::data(const QModelIndex &index, int role) const {
  if (!((index.isValid()) &&
       (index.row() >= 0) &&
       (index.row() < rowCount())))
  {
    return {};
  }

  const auto& entry = _sources[index.row()];

  switch (role)
  {
    case RoleThumbnailUrl:
      {
        return NetworkManager::instance().resolvedPath().resolved(entry.thumbnailUrl.mid(1));
      }
    case RoleTitle:
      {
        return entry.title;
      }
    case RoleUrl:
      {
        return entry.url;
      }
    case RoleId:
      {
        return entry.id;
      }
    case RoleInitialized:
      {
        return entry.isInitialized;
      }
    case RoleInLibrary:
      {
        return entry.inLibrary;
      }
    case RoleFreshData:
      {
        return entry.freshData;
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
QHash<int, QByteArray> SourcesLibraryModel::roleNames() const {
  static QHash<int, QByteArray> roles = { {RoleThumbnailUrl,  "thumbnailUrl"},
                                          {RoleTitle,         "title"},
                                          {RoleId,            "mangaId"},
                                          {RoleUrl,           "url"},
                                          {RoleInitialized,   "isInitialized"},
                                          {RoleFreshData,     "freshData"},
                                          {RoleInLibrary,     "inLibrary"},};

  return roles;
}

/******************************************************************************
 *
 * Method: search()
 *
 *****************************************************************************/
void SourcesLibraryModel::search(const QString& searchTerm)
{
  beginResetModel();
  _resetModel = true;
  _hasNextPage = false;
  _sources.clear();
  endResetModel();

  NetworkManager::instance().get(QUrl(u"source/"_qs % _source % "/search?searchTerm=" % QUrl::toPercentEncoding(searchTerm)), this, receiveReply);
}

/******************************************************************************
 *
 * Method: next()
 *
 *****************************************************************************/
void SourcesLibraryModel::next()
{
  if (!_hasNextPage) {
    return;
  }
  NetworkManager::instance().get(QUrl(u"source/"_qs % _source % "/latest/" % QString::number(++_pageNumber)), this, receiveReply);

}
