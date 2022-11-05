#include "DownloadsModel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQmlEngine>
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
          [=](QAbstractSocket::SocketError error) {
            qDebug() << "error: " << error << _webSocket.errorString();
          });
  auto resolved = NetworkManager::instance().resolvedPath().resolved(QString("api/v1/downloads"));
  bool ssl = !resolved.scheme().compare("https", Qt::CaseInsensitive);
  resolved.setScheme(ssl ? "wss" : "ws");

  QNetworkRequest request;
  request.setUrl(resolved);
  request.setRawHeader("Authorization",
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
}

/******************************************************************************
 *
 * onTextMessageReceived
 *
 *****************************************************************************/
void DownloadsModel::onTextMessageReceived(const QString &message) {
  QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());

  beginResetModel();
  _queue.clear();

  _status = doc["status"].toString();

  for (const auto &entry_arr : doc["queue"].toArray()) {
    const auto &entry = entry_arr.toObject();
    auto &info = _queue.emplace_back();
    info.chapterIndex = entry["chapterIndex"].toInt();
    info.mangaId = entry["mangaId"].toInt();
    info.state = entry["state"].toString();
    info.progress = entry["progress"].toDouble() * 100;
    info.tries = entry["tries"].toInt();

    if (!_mangaInfo.contains(info.mangaId)) {
      NetworkManager::instance().get(QUrl(u"manga"_qs % '/' % QString::number(info.mangaId)), this,
        [&](const auto& doc)
      {
        beginResetModel();
        const auto &entry = doc.object();
        auto &info = _mangaInfo[entry["id"].toInt()];
        info.processDetails(entry);
        endResetModel();
      });
      _mangaInfo[info.mangaId] = {};
    }

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
    return _mangaInfo[entry.mangaId].title;
  }

  case RoleThumbnail: {
    return NetworkManager::instance().resolvedPath().resolved(
        _mangaInfo[entry.mangaId].thumbnailUrl.mid(1));
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
void DownloadsModel::clear() { NetworkManager::instance().get("downloads/clear"); }

/******************************************************************************
 *
 * Method: clear()
 *
 *****************************************************************************/
void DownloadsModel::pause() { NetworkManager::instance().get("downloads/stop"); }

/******************************************************************************
 *
 * Method: clear()
 *
 *****************************************************************************/
void DownloadsModel::start() { NetworkManager::instance().get("downloads/start"); }

/******************************************************************************
 *
 * Method: cancel()
 *
 *****************************************************************************/
void DownloadsModel::cancel(qint32 index) {
  NetworkManager::instance().deleteResource(QStringLiteral("download/%1/chapter/%2")
                                      .arg(_queue[index].mangaId)
                                      .arg(_queue[index].chapterIndex));
}
