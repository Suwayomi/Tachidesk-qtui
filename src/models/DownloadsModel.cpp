#include "DownloadsModel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQmlEngine>
#include <graphqlservice/JSONResponse.h>
#include <qcoreapplication.h>

#include "../networkmanager.h"

/******************************************************************************
 *
 * DownloadsModel
 *
 *****************************************************************************/
DownloadsModel::DownloadsModel(QObject *parent) : QAbstractListModel(parent) {}

/******************************************************************************
 *
 * requestChapters
 *
 *****************************************************************************/
void DownloadsModel::classBegin() {}

/******************************************************************************
 *
 * componentComplete
 *
 *****************************************************************************/
void DownloadsModel::componentComplete() { setupWebsocket(); }

/******************************************************************************
 *
 * setupWebsocket
 *
 *****************************************************************************/
void DownloadsModel::setupWebsocket() {
  connect(&_webSocket, &QWebSocket::connected, this,
          &DownloadsModel::onConnected);
  connect(&_webSocket, &QWebSocket::disconnected, this,
          &DownloadsModel::closed);
  connect(&_webSocket,
          QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
          [=, this](QAbstractSocket::SocketError error) {
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
}

/******************************************************************************
 *
 * closed
 *
 *****************************************************************************/
void DownloadsModel::closed() {}

/******************************************************************************
 *
 * onConnected
 *
 *****************************************************************************/
void DownloadsModel::onConnected() {
  connect(&_webSocket, &QWebSocket::textMessageReceived, this,
          &DownloadsModel::onTextMessageReceived);

  QJsonObject initMsg;
  initMsg["type"] = "connection_init";
  _webSocket.sendTextMessage(QJsonDocument(initMsg).toJson(QJsonDocument::Compact));

}

/******************************************************************************
 *
 * onTextMessageReceived
 *
 *****************************************************************************/
void DownloadsModel::onTextMessageReceived(const QString &message) {
  QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
  qDebug() << "DownloadsModel: Received message:" << doc;
  // check if we received connection_awk
  if (doc.isEmpty() || !doc.isObject()) {
    qDebug() << "DownloadsModel: Invalid message received:" << message;
    return;
  }
  if (doc["type"].toString() == "connection_ack") {
    /*
     * {"id":"bf79028d-9fbd-4858-b43f-c1869bf0fe9e","type":"subscribe","payload":{"variables":{"input":{"maxUpdates":30}},"extensions":{},"operationName":"DOWNLOAD_STATUS_SUBSCRIPTION","query":"fragment DOWNLOAD_TYPE_FIELDS on DownloadType {\n  chapter {\n    id\n    name\n    sourceOrder\n    isDownloaded\n    __typename\n  }\n  manga {\n    id\n    title\n    downloadCount\n    __typename\n  }\n  progress\n  state\n  tries\n  __typename\n}\n\nfragment DOWNLOAD_UPDATES_FIELDS on DownloadUpdates {\n  state\n  omittedUpdates\n  updates {\n    type\n    download {\n      ...DOWNLOAD_TYPE_FIELDS\n      position\n      __typename\n    }\n    __typename\n  }\n  __typename\n}\n\nsubscription DOWNLOAD_STATUS_SUBSCRIPTION($input: DownloadChangedInput!) {\n  downloadStatusChanged(input: $input) {\n    ...DOWNLOAD_UPDATES_FIELDS\n    __typename\n  }\n}"}}
    */

    qDebug() << "DownloadsModel: WebSocket connection established";
    // send  DOWNLOAD_STATUS_SUBSCRIBE message to subscribe for downloads
    QJsonObject subscribeMsg;
    QJsonObject payload;
    QJsonObject input;
    input["maxUpdates"] = 30; // max updates to receive
    QJsonObject variables;
    variables["input"] = input;
    payload["variables"] = variables;
    payload["extensions"] = QJsonObject();
    payload["operationName"] = QString::fromStdString(graphql::client::subscription::DOWNLOAD_STATUS_SUBSCRIPTION::GetOperationName());
    payload["query"] = QString::fromStdString(graphql::client::subscription::DOWNLOAD_STATUS_SUBSCRIPTION::GetRequestText());
    subscribeMsg["id"] = QUuid::createUuid().toString(QUuid::WithoutBraces);
    subscribeMsg["type"] = "subscribe";
    subscribeMsg["payload"] = payload;

    _webSocket.sendTextMessage(QJsonDocument(subscribeMsg).toJson(QJsonDocument::Compact));
    qDebug() << "DownloadsModel: sent DOWNLOAD_STATUS_SUBSCRIBE message" << subscribeMsg;

    return;
  }


  beginResetModel();
  _queue.clear();

  graphql::response::Value gqlResponse =
      graphql::response::parseJSON(doc["payload"].toObject().value("data").toString().toStdString());
  graphql::response::Value data = std::move(gqlResponse);
  graphql::client::subscription::DOWNLOAD_STATUS_SUBSCRIPTION::Response parsed =
      graphql::client::subscription::DOWNLOAD_STATUS_SUBSCRIPTION::parseResponse(
          std::move(data));
  qDebug() << "DownloadsModel: parsed response" << (int)parsed.downloadStatusChanged.state;

  _status = doc["status"].toString();

  for (const auto &entry_arr : doc["queue"].toArray()) {
    const auto &entry = entry_arr.toObject();
    auto &info = _queue.emplace_back();
    info.chapterIndex = entry["chapterIndex"].toInt();
    info.mangaId = entry["mangaId"].toInt();
    info.state = entry["state"].toString();
    info.progress = entry["progress"].toDouble() * 100;
    info.tries = entry["tries"].toInt();
    const auto &manga = entry["manga"];
    info.title = manga["title"].toString();
    info.thumbnailUrl = manga["thumbnailUrl"].toString();

    info.chapterInfo.processChapter(entry["chapter"].toObject());
  }

  endResetModel();

  emit downloadsUpdated(_queue);
}

/******************************************************************************
 *
 * Method: rowCount()
 *
 *****************************************************************************/
int DownloadsModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return 0;
  }

  return _queue.size();
}

/******************************************************************************
 *
 * Method: data()
 *
 *****************************************************************************/
QVariant DownloadsModel::data(const QModelIndex &index, int role) const {
  if (!((index.isValid()) && (index.row() >= 0) &&
        (index.row() < rowCount()))) {
    return {};
  }

  const auto &entry = _queue[index.row()];

  switch (role) {
  case RoleUrl: {
    return entry.chapterInfo.url;
  }
  case RoleName: {
    return entry.chapterInfo.name;
  }
  case RoleChapterNumber: {
    return entry.chapterInfo.chapterNumber;
  }
  case RoleRead: {
    return entry.chapterInfo.read;
  }
  case RoleChapterIndex: {
    return entry.chapterInfo.index;
  }
  case RolePageCount: {
    return entry.chapterInfo.pageCount;
  }
  case RoleChapterCount: {
    return entry.chapterInfo.chapterCount;
  }
  case RoleFetchedAt: {
    return entry.chapterInfo.fetchedAt;
  }

  case RoleLastPageRead: {
    return entry.chapterInfo.lastPageRead;
  }

  case RoleDownloaded: {
    return entry.chapterInfo.downloaded;
  }
  case RoleMangaId: {
    return entry.mangaId;
  }
  case RoleState: {
    return entry.state;
  }
  case RoleProgress: {
    return entry.progress;
  }
  case RoleTries: {
    return entry.tries;
  }

  case RoleTitle: {
    return entry.title;
  }

  case RoleThumbnail: {
    return NetworkManager::instance().resolvedPath().resolved(entry.thumbnailUrl.mid(1));
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
QHash<int, QByteArray> DownloadsModel::roleNames() const {
  static QHash<int, QByteArray> roles = {
      {RoleUrl, "url"},
      {RoleName, "name"},
      {RoleChapterNumber, "chapterNumber"},
      {RoleRead, "read"},
      {RoleChapterIndex, "chapterIndex"},
      {RolePageCount, "pageCount"},
      {RoleDownloaded, "downloaded"},
      {RoleLastPageRead, "lastPageRead"},
      {RoleChapterCount, "chapterCount"},
      {RoleFetchedAt, "fetchedAt"},

      {RoleMangaId, "mangaId"},
      {RoleState, "state"},
      {RoleProgress, "progress"},
      {RoleTries, "tries"},

      {RoleTitle, "title"},
      {RoleThumbnail, "thumbnailUrl"},
  };

  return roles;
}

/******************************************************************************
 *
 * Method: clear()
 *
 *****************************************************************************/
void DownloadsModel::clear() {
  NetworkManager::instance().get("downloads/clear");
}

/******************************************************************************
 *
 * Method: clear()
 *
 *****************************************************************************/
void DownloadsModel::pause() {
  NetworkManager::instance().get("downloads/stop");
}

/******************************************************************************
 *
 * Method: clear()
 *
 *****************************************************************************/
void DownloadsModel::start() {
  NetworkManager::instance().get("downloads/start");
}

/******************************************************************************
 *
 * Method: cancel()
 *
 *****************************************************************************/
void DownloadsModel::cancel(qint32 index) {
  NetworkManager::instance().deleteResource(
      QStringLiteral("download/%1/chapter/%2")
          .arg(_queue[index].mangaId)
          .arg(_queue[index].chapterIndex));
}
