#include "SourcesLibraryModel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <qcoreapplication.h>
#include <QQmlEngine>

#define exportQmlType(ns, cls) qmlRegisterType<cls>(#ns, 1, 0, #cls)
#define IMPLEMENT_QTQUICK_TYPE(ns, cls) \
  void register ## cls ## Type() { exportQmlType(ns, cls); } \
  Q_COREAPP_STARTUP_FUNCTION(register ## cls ## Type)

IMPLEMENT_QTQUICK_TYPE(Tachidesk.Models, SourcesLibraryModel)

/******************************************************************************
 *
 * SourcesLibraryModel
 *
 *****************************************************************************/
SourcesLibraryModel::SourcesLibraryModel(QObject* parent)
  : QAbstractListModel(parent)
{
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
 * Method: receiveReply()
 *
 *****************************************************************************/
void SourcesLibraryModel::receivedReply(const QJsonDocument& reply)
{
  disconnect(_networkManager, &NetworkManager::receivedReply, this, nullptr);

  const auto& mangaList = reply["mangaList"].toArray();

  if (resetModel) {
    beginResetModel();
    _sources.clear();
  } else {
    beginInsertRows({}, _sources.size(), _sources.size() + mangaList.count() - 1);
  }


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

  if (resetModel) {
    resetModel = false;
    endResetModel();
  } else {
    endInsertRows();
  }
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
        return _networkManager->resolvedPath().arg(entry.thumbnailUrl);
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
  resetModel = true;
  _networkManager->get(QStringLiteral("source/%1/search/%2/1").arg(_source).arg(searchTerm));

  connect(
      _networkManager,
      &NetworkManager::receivedReply,
      this,
      &SourcesLibraryModel::receivedReply);
}

/******************************************************************************
 *
 * Method: next()
 *
 *****************************************************************************/
void SourcesLibraryModel::next()
{
  _networkManager->get(QStringLiteral("source/%1/latest/%2").arg(_source).arg(++_pageNumber));

  connect(
      _networkManager,
      &NetworkManager::receivedReply,
      this,
      &SourcesLibraryModel::receivedReply);
}
