#include "DownloadsModel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <qcoreapplication.h>
#include <QQmlEngine>

#define exportQmlType(ns, cls) qmlRegisterType<cls>(#ns, 1, 0, #cls)
#define IMPLEMENT_QTQUICK_TYPE(ns, cls) \
  void register ## cls ## Type() { exportQmlType(ns, cls); } \
  Q_COREAPP_STARTUP_FUNCTION(register ## cls ## Type)

IMPLEMENT_QTQUICK_TYPE(Tachidesk.Models, DownloadsModel)

/******************************************************************************
 *
 * DownloadsModel
 *
 *****************************************************************************/
DownloadsModel::DownloadsModel(QObject* parent)
  : QAbstractListModel(parent)
{
}

/******************************************************************************
 *
 * requestChapters
 *
 *****************************************************************************/
void DownloadsModel::classBegin()
{
}

/******************************************************************************
 *
 * componentComplete
 *
 *****************************************************************************/
void DownloadsModel::componentComplete()
{
  setupWebsocket();
}

/******************************************************************************
 *
 * setupWebsocket
 *
 *****************************************************************************/
void DownloadsModel::setupWebsocket()
{
  connect(
      _networkManager,
      &NetworkManager::receivedReply,
      this,
      &DownloadsModel::gotDetails);

  connect(&_webSocket, &QWebSocket::connected, this, &DownloadsModel::onConnected);
  connect(&_webSocket, &QWebSocket::disconnected, this, &DownloadsModel::closed);
  auto resolved = _networkManager->resolvedPath().arg("/api/v1/downloads");
  bool ssl = resolved.startsWith("https", Qt::CaseInsensitive);
  resolved = resolved.mid(resolved.indexOf('/', resolved.indexOf(':'))+2);
  resolved = QStringLiteral("%1://%2")
    .arg(ssl ? "wss" : "ws")
    .arg(resolved);

  _webSocket.open(QUrl(resolved));
}

/******************************************************************************
 *
 * closed
 *
 *****************************************************************************/
void DownloadsModel::closed()
{
}

/******************************************************************************
 *
 * onConnected
 *
 *****************************************************************************/
void DownloadsModel::onConnected()
{
  connect(&_webSocket, &QWebSocket::textMessageReceived,
          this, &DownloadsModel::onTextMessageReceived);
}

/******************************************************************************
 *
 * onTextMessageReceived
 *
 *****************************************************************************/
void DownloadsModel::onTextMessageReceived(const QString& message)
{
  QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());

  beginResetModel();
  _queue.clear();

  _status = doc["status"].toString();

  for (const auto& entry_arr : doc["queue"].toArray()) {
    const auto& entry = entry_arr.toObject();
    auto& info        = _queue.emplace_back();
    info.chapterIndex = entry["chapterIndex"].toInt();
    info.mangaId      = entry["mangaId"].toInt();
    info.state        = entry["state"].toString();
    info.progress     = entry["progress"].toDouble() * 100;
    info.tries        = entry["tries"].toInt();

    if (!_mangaInfo.contains(info.mangaId)) {
      _networkManager->get(QStringLiteral("manga/%1").arg(info.mangaId));
      _mangaInfo[info.mangaId] = {};
    }

    info.chapterInfo.processChapter(entry["chapter"].toObject());
  }

  endResetModel();

  emit downloadsUpdated(_queue);
}

/******************************************************************************
 *
 * gotDetails
 *
 *****************************************************************************/
void DownloadsModel::gotDetails(const QJsonDocument& reply)
{
  beginResetModel();
  const auto& entry = reply.object();
  auto& info        = _mangaInfo[entry["id"].toInt()];
  info.processDetails(entry);
  endResetModel();
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
  if (!((index.isValid()) &&
       (index.row() >= 0) &&
       (index.row() < rowCount())))
  {
    return {};
  }

  const auto& entry = _queue[index.row()];

  switch (role)
  {
    case RoleUrl:
      {
        return entry.chapterInfo.url;
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
    case RoleFetchedAt:
      {
        return entry.chapterInfo.fetchedAt;
      }

    case RoleLastPageRead:
      {
        return entry.chapterInfo.lastPageRead;
      }

    case RoleDownloaded:
      {
        return entry.chapterInfo.downloaded;
      }
    case RoleMangaId:
      {
        return entry.mangaId;
      }
    case RoleState:
      {
        return entry.state;
      }
    case RoleProgress:
      {
        return entry.progress;
      }
    case RoleTries:
      {
        return entry.tries;
      }

    case RoleTitle:
      {
        return _mangaInfo[entry.mangaId].title;
      }

    case RoleThumbnail:
      {
        return _networkManager->resolvedPath().arg(_mangaInfo[entry.mangaId].thumbnailUrl);
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
QHash<int, QByteArray> DownloadsModel::roleNames() const {
  static QHash<int, QByteArray> roles = { {RoleUrl,           "url"},
                                          {RoleName,          "name"},
                                          {RoleChapterNumber, "chapterNumber"},
                                          {RoleRead,          "read"},
                                          {RoleChapterIndex,  "chapterIndex"},
                                          {RolePageCount,     "pageCount"},
                                          {RoleDownloaded,    "downloaded"},
                                          {RoleLastPageRead,  "lastPageRead"},
                                          {RoleChapterCount,  "chapterCount"},
                                          {RoleFetchedAt,     "fetchedAt"},

                                          {RoleMangaId,       "mangaId"},
                                          {RoleState,         "state"},
                                          {RoleProgress,      "progress"},
                                          {RoleTries,         "tries"},

                                          {RoleTitle,         "title"},
                                          {RoleThumbnail,     "thumbnailUrl"},
  };

  return roles;
}

/******************************************************************************
 *
 * Method: clear()
 *
 *****************************************************************************/
void DownloadsModel::clear()
{
  _networkManager->get("downloads/clear");
}

/******************************************************************************
 *
 * Method: clear()
 *
 *****************************************************************************/
void DownloadsModel::pause()
{
  _networkManager->get("downloads/stop");
}

/******************************************************************************
 *
 * Method: clear()
 *
 *****************************************************************************/
void DownloadsModel::start()
{
  _networkManager->get("downloads/start");
}

/******************************************************************************
 *
 * Method: cancel()
 *
 *****************************************************************************/
void DownloadsModel::cancel(qint32 index)
{
  _networkManager->deleteResource(
      QStringLiteral("download/%1/chapter/%2")
        .arg(_queue[index].mangaId)
        .arg(_queue[index].chapterIndex));
}
