
#include "UpdatesModel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <qcoreapplication.h>
#include <QQmlEngine>

#define exportQmlType(ns, cls) qmlRegisterType<cls>(#ns, 1, 0, #cls)
#define IMPLEMENT_QTQUICK_TYPE(ns, cls) \
  void register ## cls ## Type() { exportQmlType(ns, cls); } \
  Q_COREAPP_STARTUP_FUNCTION(register ## cls ## Type)

IMPLEMENT_QTQUICK_TYPE(Tachidesk.Models, UpdatesModel)

/******************************************************************************
 *
 * UpdatesModel
 *
 *****************************************************************************/
UpdatesModel::UpdatesModel(QObject* parent)
  : QAbstractListModel(parent)
{
}

void UpdatesModel::classBegin() { }

/******************************************************************************
 *
 * componentComplete
 *
 *****************************************************************************/
void UpdatesModel::componentComplete()
{
  next();
}

/******************************************************************************
 *
 * Method: recieveReply()
 *
 *****************************************************************************/
void UpdatesModel::recievedReply(const QJsonDocument& reply)
{
  disconnect(_networkManager, &NetworkManager::recievedReply, this, nullptr);

  beginInsertRows({}, _sources.size(), _sources.size() + reply.array().count() - 1);

  for (const auto& entry_arr : reply.array()) {
    const auto& entry   = entry_arr.toObject();
    auto& info          = _sources.emplace_back();
    info.id             = entry["id"].toInt();
    info.sourceId       = entry["sourceId"].toInt();
    info.title          = entry["title"].toString();
    info.thumbnailUrl   = entry["thumbnailUrl"].toString();
    info.url            = entry["url"].toString();
    info.isInitialized  = entry["isInitialized"].toBool();
    info.inLibrary      = entry["inLibrary"].toBool();

    info.chapterInfo.processChapter(entry["chapter"].toObject());
  }

  endInsertRows();
}

/******************************************************************************
 *
 * Method: rowCount()
 *
 *****************************************************************************/
int UpdatesModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return 0;
  }
  qDebug() << "sources size: " << _sources.size();

  return _sources.size();
}

/******************************************************************************
 *
 * Method: data()
 *
 *****************************************************************************/
QVariant UpdatesModel::data(const QModelIndex &index, int role) const {
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
    case RoleChapterUrl:
      {
        return entry.url;
      }
    case RoleName:
      {
        return entry.chapterInfo.name;
      }
    case RoleChapterNumber:
      {
        return entry.chapterInfo.chapterNumber;
      }
    case RoleRead:
      {
        return entry.chapterInfo.read;
      }
    case RoleChapterIndex:
      {
        return entry.chapterInfo.index;
      }
    case RolePageCount:
      {
        return entry.chapterInfo.pageCount;
      }
    case RoleChapterCount:
      {
        return entry.chapterInfo.chapterCount;
      }

    case RoleLastPageRead:
      {
        return entry.chapterInfo.lastPageRead;
      }

    case RoleDownloaded:
      {
        return entry.chapterInfo.downloaded;
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
QHash<int, QByteArray> UpdatesModel::roleNames() const {
  static QHash<int, QByteArray> roles = { {RoleThumbnailUrl,  "thumbnailUrl"},
                                          {RoleTitle,         "title"},
                                          {RoleId,            "mangaId"},
                                          {RoleUrl,           "url"},
                                          {RoleInitialized,   "isInitialized"},
                                          {RoleFreshData,     "freshData"},
                                          {RoleInLibrary,     "inLibrary"},
                                          {RoleChapterUrl,    "chapterUrl"},
                                          {RoleName,          "name"},
                                          {RoleChapterNumber, "chapterNumber"},
                                          {RoleRead,          "read"},
                                          {RoleChapterIndex,  "chapterIndex"},
                                          {RolePageCount,     "pageCount"},
                                          {RoleDownloaded,    "downloaded"},
                                          {RoleLastPageRead,  "lastPageRead"},
                                          {RoleChapterCount,  "chapterCount"},
  };

  return roles;
}

/******************************************************************************
 *
 * Method: next()
 *
 *****************************************************************************/
void UpdatesModel::next()
{
  _networkManager->get(QStringLiteral("update/recentChapters"));

  connect(
      _networkManager,
      &NetworkManager::recievedReply,
      this,
      &UpdatesModel::recievedReply);
}
