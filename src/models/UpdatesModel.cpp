
#include "UpdatesModel.h"
#include "DownloadsModel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <qcoreapplication.h>
#include <QQmlEngine>
#include <qurlquery.h>

#include "../networkmanager.h"

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
  connect(&_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
  [=](QAbstractSocket::SocketError error){
    qDebug() << "error: " << error << _webSocket.errorString();
  });

  auto resolved = NetworkManager::instance().resolvedPath().resolved(QString("api/v1/update"));
  bool ssl = !resolved.scheme().compare("https", Qt::CaseInsensitive);
  resolved.setScheme(ssl ? "wss" : "ws");

  QNetworkRequest request;
  request.setUrl(resolved);
  request.setRawHeader("Authorization", QString("Basic %1").arg(QByteArray(QString("%1:%2").arg(NetworkManager::instance().username()).arg(NetworkManager::instance().password()).toUtf8()).toBase64()).toUtf8());

  _webSocket.open(request);

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
        source.chapterInfo.downloadPrepairing = false;
        emit dataChanged(createIndex(row, 0), createIndex(row, 0), { RoleDownloadProgress, RoleDownloaded, RoleDownloadPrepairing});
        break;
      }
      row++;
    }
  }
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
    case RoleDownloadPrepairing:
      return entry.chapterInfo.downloadPrepairing.value_or(false);

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
                                          {RoleDownloadPrepairing,"downloadPrepairing"},
  };

  return roles;
}

/******************************************************************************
 *
 * Method: pageRefresh()
 *
 *****************************************************************************/
void UpdatesModel::pageRefresh()
{
  _pageNumber = 0;
  _hasNext = false;
  _sources.clear();
  beginResetModel();
  next();
  endResetModel();
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

  NetworkManager::instance().get(QUrl(u"update/recentChapters/"_qs % QString::number(_pageNumber++)), this,
    [&](const auto& reply)
  {
    if (!downloads) {
      downloads = std::make_shared<DownloadsModel>();
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
  });
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
  NetworkManager::instance().post("update/fetch", query);
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
  // mark as downloaded so we don't download more than once
  entry.chapterInfo.downloaded = true;
  entry.chapterInfo.downloadPrepairing = true;

  qDebug() << "source index? " << index;
  emit dataChanged(createIndex(index, 0), createIndex(index, 0), { RoleDownloadPrepairing });
  NetworkManager::instance().get(QStringLiteral("download/%1/chapter/%2").arg(entry.id).arg(entry.chapterInfo.index));
}

/******************************************************************************
 *
 * Method: chapterRead()
 *
 *****************************************************************************/
void UpdatesModel::chapterRead(qint32 mangaId, quint32 chapter)
{
  int i = 0;
  for (auto& info : _sources) {
    if (info.id == mangaId &&
        info.chapterInfo.index == chapter)
    {
      info.chapterInfo.read = true;
      NetworkManager::instance().patch("read", "true",
        QStringLiteral("manga/%1/chapter/%2").arg(mangaId).arg(chapter));
      emit dataChanged(createIndex(i, 0), createIndex(i, 0), { RoleRead });
      break;
    }
    ++i;
  }
}
