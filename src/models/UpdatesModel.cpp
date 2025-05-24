
#include "UpdatesModel.h"
#include "DownloadsModel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQmlEngine>
#include <qcoreapplication.h>
#include <qurlquery.h>

#include "../networkmanager.h"

/******************************************************************************
 *
 * UpdatesModel
 *
 *****************************************************************************/
UpdatesModel::UpdatesModel(QObject *parent) : QAbstractListModel(parent) {}

/******************************************************************************
 *
 * closed
 *
 *****************************************************************************/
void UpdatesModel::closed() {}

/******************************************************************************
 *
 * onConnected
 *
 *****************************************************************************/
void UpdatesModel::onConnected() {
  connect(&_webSocket, &QWebSocket::textMessageReceived, this,
          &UpdatesModel::onTextMessageReceived);
}

/******************************************************************************
 *
 * onTextMessageReceived
 *
 *****************************************************************************/
void UpdatesModel::onTextMessageReceived(const QString &message) {
  QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());

  //{"statusMap":{"COMPLETE" : 26,"FAILED" : 69,"RUNNING" : 1,"PENDING" : 123},
  //"running":true}
  _running = doc["running"].toBool();

  auto statusMap = doc["statusMap"].toObject();
  _complete = statusMap["COMPLETE"].toArray().count();
  _complete += statusMap["FAILED"].toArray().count();

  _total = statusMap["FAILED"].toArray().count();
  _total += statusMap["COMPLETE"].toArray().count();
  _total += statusMap["RUNNING"].toArray().count();
  _total += statusMap["PENDING"].toArray().count();

  emit totalChanged();
  emit completeChanged();
  emit runningChanged();
}

void UpdatesModel::classBegin() {}

/******************************************************************************
 *
 * componentComplete
 *
 *****************************************************************************/
void UpdatesModel::componentComplete() {
  connect(&_webSocket, &QWebSocket::connected, this,
          &UpdatesModel::onConnected);
  connect(&_webSocket, &QWebSocket::disconnected, this, &UpdatesModel::closed);
  connect(&_webSocket,
          QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
          [=,this](QAbstractSocket::SocketError error) {
            qDebug() << "error: " << error << _webSocket.errorString();
          });

  auto resolved = NetworkManager::instance().resolvedPath().resolved(
      QString("api/v1/update"));
  bool ssl = !resolved.scheme().compare("https", Qt::CaseInsensitive);
  resolved.setScheme(ssl ? "wss" : "ws");

  QNetworkRequest request;
  request.setUrl(resolved);
  request.setRawHeader(
      "Authorization",
      QString("Basic %1")
          .arg(QByteArray(QString("%1:%2")
                              .arg(NetworkManager::instance().username())
                              .arg(NetworkManager::instance().password())
                              .toUtf8())
                   .toBase64())
          .toUtf8());

  _webSocket.open(request);

  next();
}

/******************************************************************************
 *
 * Method: onDownloadsUpdated()
 *
 *****************************************************************************/
void UpdatesModel::onDownloadsUpdated(const std::vector<QueueInfo> &queueInfo) {
  for (auto &info : queueInfo) {
    auto source = _queueInfo.find(info.mangaId);
    if (!source->second) {
      source->second = std::make_shared<QueueInfo>(info);
      source->second->chapterInfo.downloaded = info.progress >= 100;
      source->second->downloadPrepairing = false;

      auto it = std::find_if(_entries.chapters.nodes.begin(),
                             _entries.chapters.nodes.end(),
                             [&info](const auto &entry) {
                               return entry.manga.id == info.mangaId;
                             });
      size_t index = std::distance(_entries.chapters.nodes.begin(), it);
      emit dataChanged(
          createIndex(index, 0), createIndex(index, 0),
          {RoleDownloadProgress, RoleDownloaded, RoleDownloadPrepairing});
    }
  }
}

/******************************************************************************
 *
 * Method: rowCount()
 *
 *****************************************************************************/
int UpdatesModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid() || _entries.chapters.nodes.empty()) {
    return 0;
  }

  return _entries.chapters.nodes.size();
}

/******************************************************************************
 *
 * Method: data()
 *
 *****************************************************************************/
QVariant UpdatesModel::data(const QModelIndex &index, int role) const {
  if (!((index.isValid()) && (index.row() >= 0) &&
        (index.row() < rowCount())) || _entries.chapters.nodes.empty()) {
    return {};
  }

  const auto &entry = _entries.chapters.nodes[index.row()];
  const auto &queueInfo = _queueInfo.find(entry.manga.id);

  switch (role) {
  case RoleThumbnailUrl: {
    return NetworkManager::instance().resolvedPath().resolved(
        QString::fromStdString(entry.manga.thumbnailUrl.value_or("")));
  }
  case RoleTitle: {
    return entry.manga.title.c_str();
  }
  case RoleUrl: {
    return entry.realUrl.value_or("").c_str();
  }
  case RoleId: {
    return entry.manga.id;
  }
  case RoleInitialized: {
    return entry.manga.initialized;
  }
  case RoleInLibrary: {
    return entry.manga.inLibrary;
  }
  // case RoleFreshData: {
  //   return entry.manga.freshData;
  // }
  case RoleChapterUrl: {
    return "";
  }
  case RoleName: {
    return entry.name.c_str();
  }
  case RoleChapterNumber: {
    return entry.chapterNumber;
  }
  case RoleRead: {
    return entry.isRead;
  }
  case RoleChapterIndex: {
    return entry.sourceOrder;
  }

  case RoleChapterId: {
    return entry.id;
  }
  // case RolePageCount: {
  //   return entry.chapterInfo.pageCount;
  // }
  // not sure this is even used?
  case RoleChapterCount: {
    return entry.sourceOrder;
  }

  // case RoleLastPageRead: {
  //   return entry.lastPageRead;
  // }

  case RoleDownloaded: {
    return entry.isDownloaded;
  }

  case RoleFetchedAt: {
    return QString(entry.fetchedAt.get<std::string>().c_str());
  }

  case RoleDownloadProgress: {
    if (queueInfo == _queueInfo.end() || !queueInfo->second) {
      return -1;
    }
    return queueInfo->second->progress;
  }
  case RoleDownloadPrepairing:
    if (queueInfo == _queueInfo.end() || !queueInfo->second) {
      return false;
    }
    return queueInfo->second->downloadPrepairing;
    //return !entry.isDownloaded; //.downloadPrepairing.value_or(false);

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
  static QHash<int, QByteArray> roles = {
      {RoleThumbnailUrl, "thumbnailUrl"},
      {RoleTitle, "title"},
      {RoleId, "mangaId"},
      {RoleUrl, "url"},
      {RoleInitialized, "isInitialized"},
      {RoleFreshData, "freshData"},
      {RoleInLibrary, "inLibrary"},
      {RoleChapterUrl, "chapterUrl"},
      {RoleName, "name"},
      {RoleChapterNumber, "chapterNumber"},
      {RoleRead, "read"},
      {RoleChapterIndex, "chapterIndex"},
      {RoleChapterId, "chapterId"},
      {RolePageCount, "pageCount"},
      {RoleDownloaded, "downloaded"},
      {RoleLastPageRead, "lastPageRead"},
      {RoleChapterCount, "chapterCount"},
      {RoleFetchedAt, "fetchedAt"},
      {RoleDownloadProgress, "downloadProgress"},
      {RoleDownloadPrepairing, "downloadPrepairing"},
  };

  return roles;
}

/******************************************************************************
 *
 * Method: pageRefresh()
 *
 *****************************************************************************/
void UpdatesModel::pageRefresh() {
  _pageNumber = 0;
  _hasNext = false;
  _entries.chapters.nodes.clear();
  beginResetModel();
  next();
  endResetModel();
}
/******************************************************************************
 *
 * Method: next()
 *
 *****************************************************************************/
void UpdatesModel::next() {
  if (_isRequesting) {
    return;
  }

  _isRequesting = true;

  QJsonObject variablesObj;
  variablesObj.insert("first", 50);
  variablesObj.insert("offset", 50 * _pageNumber++);

  QJsonObject filterObj;
  QJsonObject inLibraryObj;
  inLibraryObj.insert("equalTo", true);
  filterObj.insert("inLibrary", inLibraryObj);
  variablesObj.insert("filter", filterObj);

  QJsonArray orderArray;
  {
      QJsonObject order1;
      order1.insert("by", "FETCHED_AT");
      order1.insert("byType", "DESC");
      orderArray.append(order1);

      QJsonObject order2;
      order2.insert("by", "SOURCE_ORDER");
      order2.insert("byType", "DESC");
      orderArray.append(order2);
  }
  variablesObj.insert("order", orderArray);

  NetworkManager::instance().postGraphQL(graphql::client::query::GET_CHAPTERS_UPDATES::GetOperationName(), std::move(variablesObj),
    [&](graphql::response::Value&& data) {
      if (!downloads) {
        downloads = std::make_shared<DownloadsModel>();
        downloads->setupWebsocket();
        connect(downloads.get(), &DownloadsModel::downloadsUpdated, this,
                &UpdatesModel::onDownloadsUpdated);
      }
      _isRequesting = false;

      auto parsed = graphql::client::query::GET_CHAPTERS_UPDATES::parseResponse(std::move(data));

      if (_entries.chapters.nodes.empty()) {
        beginResetModel();
        _entries = std::move(parsed);
        endResetModel();
      }
      else {
        beginInsertRows({}, _entries.chapters.nodes.size(),
                        _entries.chapters.nodes.size() + parsed.chapters.nodes.size() - 1);
        std::copy(parsed.chapters.nodes.begin(), parsed.chapters.nodes.end(), std::back_inserter(_entries.chapters.nodes));
        endInsertRows();
      }
    });
}

/******************************************************************************
 *
 * Method: refresh()
 *
 *****************************************************************************/
void UpdatesModel::refresh() {
  QUrlQuery query;
  query.addQueryItem("category", 0);
  NetworkManager::instance().post("update/fetch", query);
}

/******************************************************************************
 *
 * Method: downloadChapter()
 *
 *****************************************************************************/
void UpdatesModel::downloadChapter(int index) {
  auto &entry = _entries.chapters.nodes[index];
  if (entry.isDownloaded) {
    return;
  }
  // mark as downloaded so we don't download more than once
  entry.isDownloaded = true;
  // entry.downloadPrepairing = true;

  emit dataChanged(createIndex(index, 0), createIndex(index, 0),
                   {RoleDownloadPrepairing});
  NetworkManager::instance().get(QStringLiteral("download/%1/chapter/%2")
                                     .arg(entry.id)
                                     .arg(entry.sourceOrder));
}

/******************************************************************************
 *
 * Method: chapterRead()
 *
 *****************************************************************************/
void UpdatesModel::chapterRead(qint32 mangaId, int chapter) {
  int i = 0;
  for (auto &info : _entries.chapters.nodes) {
    if (info.id == mangaId && info.sourceOrder == chapter) {
      info.isRead = true;
      NetworkManager::instance().patch(
          "read", "true",
          QStringLiteral("manga/%1/chapter/%2").arg(mangaId).arg(chapter));
      emit dataChanged(createIndex(i, 0), createIndex(i, 0), {RoleRead});
      break;
    }
    ++i;
  }
}
