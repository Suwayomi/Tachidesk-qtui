#include "UpdatesModel.h"
#include "DownloadsModel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQmlEngine>
#include <qcoreapplication.h>
#include <qurlquery.h>

#include <graphqlservice/GraphQLResponse.h>
#include <graphqlservice/GraphQLParse.h>
#include <graphqlservice/JSONResponse.h>

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
void UpdatesModel::closed() {
  _updateStatusTimer.stop();
}

/******************************************************************************
 *
 * onConnected
 *
 *****************************************************************************/
void UpdatesModel::onConnected() {
  QJsonObject initMsg;
  initMsg["type"] = "connection_init";
  _webSocket.sendTextMessage(QJsonDocument(initMsg).toJson(QJsonDocument::Compact));

  connect(&_webSocket, &QWebSocket::textMessageReceived, this,
          &UpdatesModel::onTextMessageReceived);

  // Subscribe to download status updates
  QTimer::singleShot(1000, this, [this]() {
    QJsonObject subscribeMsg;
    subscribeMsg["id"] = "download-subscription";
    subscribeMsg["type"] = "start";

    QJsonObject payload;
    payload["query"] = "subscription { downloadStatusChanged(input: { maxUpdates: 100 }) { updates { type download { progress state chapter { id } } } } }";
    subscribeMsg["payload"] = payload;

    _webSocket.sendTextMessage(QJsonDocument(subscribeMsg).toJson(QJsonDocument::Compact));
    qDebug() << "Subscribed to download status updates";
  });

  // Start polling for updates
  _updateStatusTimer.start();
}

/******************************************************************************
 *
 * onTextMessageReceived
 *
 *****************************************************************************/
void UpdatesModel::onTextMessageReceived(const QString &message) {
  QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
  QJsonObject obj = doc.object();

  // Handle GraphQL subscription messages
  QString type = obj["type"].toString();

  if (type == "connection_ack") {
    qDebug() << "WebSocket connection acknowledged";
    // Request initial status and set up periodic updates
    requestUpdateStatus();
    return;
  }

  if (type == "next") {
    QJsonObject payload = obj["payload"].toObject();
    QJsonObject data = payload["data"].toObject();

    // Handle download status updates from subscription
    if (data.contains("downloadStatusChanged")) {
      QJsonObject downloadStatus = data["downloadStatusChanged"].toObject();
      QJsonArray updates = downloadStatus["updates"].toArray();

      for (const QJsonValue& updateValue : updates) {
        QJsonObject update = updateValue.toObject();
        QString updateType = update["type"].toString();
        QJsonObject download = update["download"].toObject();

        // Process download updates
        if (updateType == "PROGRESS" || updateType == "QUEUED" || updateType == "FINISHED") {
          processChapterUpdateProgress(download);
        }
      }
    }

    // Handle update status data from subscription
    if (data.contains("libraryUpdateStatus")) {
      handleLibraryUpdateStatus(data["libraryUpdateStatus"].toObject());
    }
    return;
  }

  // Handle direct GraphQL query response (when polling for status)
  if (obj.contains("data")) {
    QJsonObject data = obj["data"].toObject();
    if (data.contains("libraryUpdateStatus")) {
      handleLibraryUpdateStatus(data["libraryUpdateStatus"].toObject());
    }
  }
}

void UpdatesModel::processChapterUpdateProgress(const QJsonObject& download) {
  // Find the chapter in the _entries that matches the download
  int chapterId = download["chapter"].toObject()["id"].toInt();
  float progress = download["progress"].toDouble();
  QString state = download["state"].toString();

  qDebug() << "Processing download update:"
           << "Chapter ID:" << chapterId
           << "Progress:" << progress
           << "State:" << state;

  // Find the index of the chapter in _entries
  auto it = std::find_if(_entries.chapters.nodes.begin(),
                          _entries.chapters.nodes.end(),
                          [chapterId](const auto& entry) {
                            return entry.id == chapterId;
                          });

  if (it != _entries.chapters.nodes.end()) {
    size_t index = std::distance(_entries.chapters.nodes.begin(), it);
    int mangaId = it->manga.id;

    // Ensure we have a queue info entry for this chapter
    auto queueIt = _queueInfo.find(mangaId);
    if (queueIt == _queueInfo.end()) {
      _queueInfo[mangaId] = std::make_shared<QueueInfo>();
      qDebug() << "Created new QueueInfo for manga ID:" << mangaId;
    }

    auto& queueInfo = _queueInfo[mangaId];
    queueInfo->progress = progress;
    queueInfo->downloadPrepairing = (state == "QUEUED");
    queueInfo->chapterInfo.downloaded = (progress >= 100 || state == "FINISHED");

    // If download is complete, update the entry's isDownloaded flag
    if (progress >= 100 || state == "FINISHED") {
      qDebug() << "Marking chapter as downloaded in entries";
      it->isDownloaded = true;
    }

    qDebug() << "Updated download info:"
             << "Manga ID:" << mangaId
             << "Progress:" << queueInfo->progress
             << "Preparing:" << queueInfo->downloadPrepairing
             << "Downloaded:" << queueInfo->chapterInfo.downloaded
             << "Entry downloaded:" << it->isDownloaded;

    // Emit data changed signal to update the view
    emit dataChanged(
        createIndex(index, 0),
        createIndex(index, 0),
        {RoleDownloadProgress, RoleDownloadPrepairing, RoleDownloaded}
    );
  } else {
    qWarning() << "Could not find chapter with ID:" << chapterId << "in updates entries";
  }
}

void UpdatesModel::handleLibraryUpdateStatus(const QJsonObject &statusObj) {
  try {
    // Convert JSON back to GraphQL Value for parsing
    QJsonDocument tempDoc(statusObj);
    std::string jsonString = tempDoc.toJson(QJsonDocument::Compact).toStdString();
    graphql::response::Value statusValue = graphql::response::parseJSON(jsonString);

    // Parse using the proper GET_UPDATE_STATUS response structure
    auto status = graphql::client::Response<graphql::qtui::client::query::GET_UPDATE_STATUS::Response::libraryUpdateStatus_LibraryUpdateStatus>::parse(std::move(statusValue));

    // Extract progress information from jobs info
    _running = status.jobsInfo.isRunning;
    _complete = status.jobsInfo.finishedJobs;
    _total = status.jobsInfo.totalJobs;

    emit totalChanged();
    emit completeChanged();
    emit runningChanged();

  } catch (const std::exception& ex) {
    qWarning() << "Failed to parse library update status:" << ex.what();
    // Fallback to basic JSON parsing
    if (statusObj.contains("jobsInfo")) {
      QJsonObject jobsInfo = statusObj["jobsInfo"].toObject();
      _running = jobsInfo["isRunning"].toBool();
      _complete = jobsInfo["finishedJobs"].toInt();
      _total = jobsInfo["totalJobs"].toInt();
    }

    emit totalChanged();
    emit completeChanged();
    emit runningChanged();
  }
}

void UpdatesModel::requestUpdateStatus() {
  // Query the current update status
  NetworkManager::instance().postGraphQL(
    graphql::qtui::client::query::GET_UPDATE_STATUS::GetOperationName(),
    QJsonObject{},
    [this](graphql::response::Value&& data) {
      auto parsed = graphql::qtui::client::query::GET_UPDATE_STATUS::parseResponse(std::move(data));

      _running = parsed.libraryUpdateStatus.jobsInfo.isRunning;
      _complete = parsed.libraryUpdateStatus.jobsInfo.finishedJobs;
      _total = parsed.libraryUpdateStatus.jobsInfo.totalJobs;

      emit totalChanged();
      emit completeChanged();
      emit runningChanged();
    }
  );
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
      QString("api/graphql"));
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

  // Set up polling timer as fallback
  _updateStatusTimer.setInterval(2000); // Poll every 2 seconds
  connect(&_updateStatusTimer, &QTimer::timeout, this, &UpdatesModel::requestUpdateStatus);

  next();
}

/******************************************************************************
 *
 * Method: onDownloadsUpdated()
 *
 *****************************************************************************/
void UpdatesModel::onDownloadsUpdated(const std::vector<QueueInfo> &queueInfo) {
  for (const auto &info : queueInfo) {
    qDebug() << "Download update received:"
             << "Manga ID:" << info.mangaId
             << "Progress:" << info.progress
             << "State:" << info.state;

    // Update or create queue info entry
    _queueInfo[info.mangaId] = std::make_shared<QueueInfo>(info);
    _queueInfo[info.mangaId]->chapterInfo.downloaded = (info.progress >= 100);
    _queueInfo[info.mangaId]->downloadPrepairing = (info.state == "QUEUED");

    // Find the corresponding chapter in the entries and update the view
    auto it = std::find_if(_entries.chapters.nodes.begin(),
                           _entries.chapters.nodes.end(),
                           [&info](const auto &entry) {
                             return entry.manga.id == info.mangaId;
                           });

    if (it != _entries.chapters.nodes.end()) {
      size_t index = std::distance(_entries.chapters.nodes.begin(), it);
      qDebug() << "Updating view for chapter at index:" << index;

      // If download is complete, update the entry's isDownloaded flag
      if (info.progress >= 100 && info.state == "FINISHED") {
        qDebug() << "Marking chapter as downloaded in entries";
        it->isDownloaded = true;
      }

      emit dataChanged(
          createIndex(index, 0), createIndex(index, 0),
          {RoleDownloadProgress, RoleDownloaded, RoleDownloadPrepairing});
    } else {
      qDebug() << "Could not find chapter for manga ID:" << info.mangaId;
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
    // Check if chapter is downloaded OR if it's completed downloading (100% progress)
    if (entry.isDownloaded) {
      return true;
    }
    if (queueInfo != _queueInfo.end() && queueInfo->second) {
      return queueInfo->second->chapterInfo.downloaded || queueInfo->second->progress >= 100;
    }
    return false;
  }

  case RoleFetchedAt: {
    return QString(entry.fetchedAt.get<std::string>().c_str());
  }

  case RoleDownloadProgress: {
    // If already downloaded, return 100%
    if (entry.isDownloaded) {
      return 100;
    }
    // Otherwise return current download progress
    if (queueInfo == _queueInfo.end() || !queueInfo->second) {
      return -1;
    }
    return queueInfo->second->progress;
  }
  case RoleDownloadPrepairing: {
    // If already downloaded, not preparing
    if (entry.isDownloaded) {
      return false;
    }
    // Check queue info for preparing state
    if (queueInfo == _queueInfo.end() || !queueInfo->second) {
      return false;
    }
    return queueInfo->second->downloadPrepairing;
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

  // Store the current page number for this request to handle out-of-order responses
  const int currentPageNumber = _pageNumber++;

  QJsonObject variablesObj;
  variablesObj.insert("first", 50);
  variablesObj.insert("offset", 50 * currentPageNumber);

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

  NetworkManager::instance().postGraphQL(graphql::qtui::client::query::GET_CHAPTERS_UPDATES::GetOperationName(), std::move(variablesObj),
    [this, currentPageNumber](graphql::response::Value&& data) {
      if (!downloads) {
        downloads = std::make_shared<DownloadsModel>();
        downloads->setupWebsocket();
        connect(downloads.get(), &DownloadsModel::downloadsUpdated, this,
                &UpdatesModel::onDownloadsUpdated);
      }
      _isRequesting = false;

      auto parsed = graphql::qtui::client::query::GET_CHAPTERS_UPDATES::parseResponse(std::move(data));

      if (_entries.chapters.nodes.empty()) {
        beginResetModel();
        _entries = std::move(parsed);
        endResetModel();
      }
      else {
        // Calculate the correct insertion position based on page number
        const int insertPosition = currentPageNumber * 50;

        // Only insert if we haven't already processed this page and position is valid
        if (insertPosition <= static_cast<int>(_entries.chapters.nodes.size())) {
          beginInsertRows({}, insertPosition, insertPosition + parsed.chapters.nodes.size() - 1);

          // Insert at the correct position using move semantics
          auto insertIterator = _entries.chapters.nodes.begin() + insertPosition;
          for (auto&& node : parsed.chapters.nodes) {
            insertIterator = _entries.chapters.nodes.insert(insertIterator, std::move(node));
            ++insertIterator;
          }

          endInsertRows();
        }
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
  if (index < 0 || index >= _entries.chapters.nodes.size()) {
    qWarning() << "Invalid chapter index:" << index;
    return;
  }

  const auto &entry = _entries.chapters.nodes[index];

  if (entry.isDownloaded) {
    qDebug() << "Chapter is already downloaded:" << entry.id;
    return;
  }

  qDebug() << "Requesting download for chapter:"
           << "Chapter ID:" << entry.id
           << "Manga ID:" << entry.manga.id
           << "Title:" << entry.name.c_str();

  // Use GraphQL mutation to enqueue the chapter for download
  QJsonObject variablesObj;
  variablesObj.insert("id", entry.id);

  NetworkManager::instance().postGraphQL(
    "mutation EnqueueChapterDownload($id: Int!) { enqueueChapterDownload(input: { id: $id }) { clientMutationId } }",
    std::move(variablesObj),
    [this, index](graphql::response::Value&& data) {
      qDebug() << "Download enqueued successfully for chapter at index:" << index;

      // Mark as preparing for download
      auto mangaId = _entries.chapters.nodes[index].manga.id;
      if (_queueInfo.find(mangaId) == _queueInfo.end()) {
        _queueInfo[mangaId] = std::make_shared<QueueInfo>();
      }
      _queueInfo[mangaId]->downloadPrepairing = true;
      _queueInfo[mangaId]->progress = 0;

      emit dataChanged(createIndex(index, 0), createIndex(index, 0),
                       {RoleDownloadPrepairing});
    }
  );
}

/******************************************************************************
 *
 * Method: chapterRead()
 *
 *****************************************************************************/
void UpdatesModel::chapterRead(qint32 Id, int chapter) {
  QJsonObject variablesObj;
  variablesObj.insert("chapterIdsToDelete", QJsonArray());
  variablesObj.insert("deleteChapters", false);
  variablesObj.insert("getBookmarked", false);
  variablesObj.insert("getLastPageRead", true);
  variablesObj.insert("getRead", true);
  QJsonObject input;
  QJsonArray idsArray;
  idsArray.append(Id);
  input.insert("ids", idsArray);
  QJsonObject patchObj;
  patchObj.insert("isRead", true);
  patchObj.insert("lastPageRead", 0);
  input.insert("patch", patchObj);
  variablesObj.insert("input", input);
  variablesObj.insert("mangaId", -1);
  variablesObj.insert("trackProgress", false);

  NetworkManager::instance().postGraphQL(graphql::qtui::client::mutation::UPDATE_CHAPTERS::GetOperationName(), std::move(variablesObj),
    [&](graphql::response::Value&& data) {
      auto parsed = graphql::qtui::client::mutation::UPDATE_LIBRARY::parseResponse(std::move(data));
      auto it = std::find_if(_entries.chapters.nodes.begin(),
                             _entries.chapters.nodes.end(),
                             [&Id, &chapter](const auto &entry) {
                               return entry.id == Id &&
                                      entry.sourceOrder == chapter;
                             });
      if (it != _entries.chapters.nodes.end()) {
        it->isRead = true;
      }

      size_t index = std::distance(_entries.chapters.nodes.begin(), it);
      emit dataChanged(createIndex(index, 0), createIndex(index, 0), {RoleRead});
  });

}
