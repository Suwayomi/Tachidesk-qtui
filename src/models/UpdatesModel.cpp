
#include "UpdatesModel.h"
#include "DownloadsModel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <qcoreapplication.h>
#include <QQmlEngine>
#include <qurlquery.h>

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

/******************************************************************************
 *
 * closed
 *
 *****************************************************************************/
void UpdatesModel::closed()
{
}

/******************************************************************************
 *
 * onConnected
 *
 *****************************************************************************/
void UpdatesModel::onConnected()
{
  connect(&_webSocket, &QWebSocket::textMessageReceived,
          this, &UpdatesModel::onTextMessageReceived);
}

/******************************************************************************
 *
 * onTextMessageReceived
 *
 *****************************************************************************/
void UpdatesModel::onTextMessageReceived(const QString& message)
{
  QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());

  //{"statusMap":{"COMPLETE" : 26,"FAILED" : 69,"RUNNING" : 1,"PENDING" : 123}, "running":true}
  _running = doc["running"].toBool();

  auto statusMap = doc["statusMap"].toObject();
  _complete = statusMap["COMPLETE"].toInt();
  _complete += statusMap["FAILED"].toInt();

  _total = statusMap["FAILED"].toInt();
  _total += statusMap["COMPLETE"].toInt();
  _total += statusMap["RUNNING"].toInt();
  _total += statusMap["PENDING"].toInt();

  emit totalChanged();
  emit completeChanged();
  emit runningChanged();

}

void UpdatesModel::classBegin() { }

/******************************************************************************
 *
 * componentComplete
 *
 *****************************************************************************/
void UpdatesModel::componentComplete()
{
  connect(&_webSocket, &QWebSocket::connected, this, &UpdatesModel::onConnected);
  connect(&_webSocket, &QWebSocket::disconnected, this, &UpdatesModel::closed);
  connect(
      _networkManager,
      &NetworkManager::receiveUpdates,
      this,
      &UpdatesModel::receivedReply);

  auto resolved = _networkManager->resolvedPath().arg("/api/v1/update");
  bool ssl = resolved.startsWith("https", Qt::CaseInsensitive);
  resolved = resolved.mid(resolved.indexOf('/', resolved.indexOf(':'))+2);
  resolved = QStringLiteral("%1://%2")
    .arg(ssl ? "wss" : "ws")
    .arg(resolved);

  _webSocket.open(QUrl(resolved));

  next();
}

/******************************************************************************
 *
 * Method: onDownloadsUpdated()
 *
 *****************************************************************************/
void UpdatesModel::onDownloadsUpdated(const std::vector<QueueInfo>& queueInfo)
{
  for (auto& info : queueInfo) {
    int row = 0;
    for (auto& source : _sources) {
      if (info.mangaId == source.id &&
          info.chapterInfo.chapterNumber == source.chapterInfo.chapterNumber)
      {
        source.queueInfo = std::make_shared<QueueInfo>(info);
        source.chapterInfo.downloaded = info.progress >= 100;
        emit dataChanged(createIndex(row, 0), createIndex(row, 0), { RoleDownloadProgress, RoleDownloaded });
        break;
      }
      row++;
    }
  }
}

/******************************************************************************
 *
 * Method: receiveReply()
 *
 *****************************************************************************/
void UpdatesModel::receivedReply(const QJsonDocument& reply)
{
  if (!downloads) {
    downloads = std::make_shared<DownloadsModel>();
    downloads->setNetworkManager(_networkManager);
    downloads->setupWebsocket();
    connect(downloads.get(), &DownloadsModel::downloadsUpdated, this, &UpdatesModel::onDownloadsUpdated);
  }

  if (reply.isEmpty()) {
    return;
  }

  _hasNext = reply["hasNextPage"].toBool();

  auto pageArray = reply["page"].toArray();
  beginInsertRows({}, _sources.size(), _sources.size() + pageArray.count() - 1);

  _sources.reserve(reply.array().count());

  for (const auto& entry_arr : pageArray) {
    const auto& entry   = entry_arr.toObject();
    const auto& manga   = entry["manga"].toObject();
    auto& info          = _sources.emplace_back();
    info.id             = manga["id"].toInt();
    info.sourceId       = manga["sourceId"].toString();
    info.title          = manga["title"].toString();
    info.thumbnailUrl   = manga["thumbnailUrl"].toString();
    info.url            = manga["url"].toString();
    info.isInitialized  = manga["isInitialized"].toBool();
    info.inLibrary      = manga["inLibrary"].toBool();

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

    case RoleFetchedAt:
      {
        return entry.chapterInfo.fetchedAt;
      }

    case RoleDownloadProgress:
      {
        if (!entry.queueInfo) {
          return -1;
        }
        return entry.queueInfo->progress;
      }
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
                                          {RoleFetchedAt,     "fetchedAt"},
                                          {RoleDownloadProgress,"downloadProgress"},
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
  if (_pageNumber && !_hasNext) {
    return;
  }

  _networkManager->getUpdates(QStringLiteral("update/recentChapters/%1").arg(_pageNumber++));
}

/******************************************************************************
 *
 * Method: refresh()
 *
 *****************************************************************************/
void UpdatesModel::refresh()
{
  QUrlQuery query;
  query.addQueryItem("category", 0);
  _networkManager->post("update/fetch", query);
}

/******************************************************************************
 *
 * Method: downloadChapter()
 *
 *****************************************************************************/
void UpdatesModel::downloadChapter(int index)
{
  auto& entry = _sources[index];
  if (entry.chapterInfo.downloaded) {
    return;
  }
  // make as downloaded so we don't download more than once
  entry.chapterInfo.downloaded = true;

  _networkManager->get(QStringLiteral("download/%1/chapter/%2").arg(entry.id).arg(entry.chapterInfo.index));
}

void UpdatesModel::chapterRead(qint32 mangaId, qint32 chapter)
{
  for (auto& info : _sources) {
    if (info.id == mangaId) {

    }
  }
}
