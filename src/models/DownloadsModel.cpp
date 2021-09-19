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
  connect(&_webSocket, &QWebSocket::connected, this, &DownloadsModel::onConnected);
  connect(&_webSocket, &QWebSocket::disconnected, this, &DownloadsModel::closed);
  auto resolved = _networkManager->resolvedPath().arg("/api/v1/downloads");
  bool ssl = resolved.startsWith("https");
  resolved = resolved.mid(resolved.indexOf('/', resolved.indexOf(':'))+2);
  resolved = QStringLiteral("%1://%2")
    .arg(ssl ? "wss" : "ws")
    .arg(resolved);

  qDebug () << resolved;
  _webSocket.open(QUrl(resolved));
}

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

void DownloadsModel::onTextMessageReceived(const QString& message)
{
  QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());

  beginResetModel();
  _queue.clear();

  _status = doc["status"].toString();

  for (const auto& entry_arr : doc["queue"].toArray()) {
    const auto& entry = entry_arr.toObject();
    auto& info                    = _queue.emplace_back();
    info.chapterIndex = entry["chapterIndex"].toInt();
    info.mangaId      = entry["mangaId"].toInt();
    info.state        = entry["state"].toString();
    info.progress     = entry["progress"].toDouble() * 100;
    info.tries        = entry["tries"].toInt();

    const auto& chapter           = entry["chapter"].toObject();
    info.chapterInfo.url          = chapter["url"].toString();
    info.chapterInfo.name         = chapter["name"].toString();
    info.chapterInfo.chapterNumber= chapter["chapterNumber"].toInt();
    info.chapterInfo.read         = chapter["read"].toBool();
    info.chapterInfo.index        = chapter["index"].toInt();
    info.chapterInfo.pageCount    = chapter["pageCount"].toInt();
    info.chapterInfo.chapterCount = chapter["chapterCount"].toInt();
    info.chapterInfo.lastPageRead = chapter["lastPageRead"].toInt();
    info.chapterInfo.downloaded   = chapter["downloaded"].toBool();
  }

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

                                          {RoleMangaId,       "mangaId"},
                                          {RoleState,         "state"},
                                          {RoleProgress,      "progress"},
                                          {RoleTries,         "tries"},
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
