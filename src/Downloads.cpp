#include "Downloads.h"
#include "networkmanager.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <qcoreapplication.h>
#include <QQmlEngine>

/******************************************************************************
 *
 * Downloads
 *
 *****************************************************************************/
Downloads::Downloads(QObject* parent)
  : QObject(parent)
{
  connect(&_webSocket, &QWebSocket::connected, this, &Downloads::onConnected);
  connect(&_webSocket, &QWebSocket::disconnected, this, &Downloads::closed);
  auto resolved = NetworkManager::instance().resolvedPath().arg("/api/v1/downloads");
  bool ssl = resolved.startsWith("https");
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
void Downloads::closed()
{
}

/******************************************************************************
 *
 * onConnected
 *
 *****************************************************************************/
void Downloads::onConnected()
{
  connect(&_webSocket, &QWebSocket::textMessageReceived,
          this, &Downloads::onTextMessageReceived);
}

/******************************************************************************
 *
 * onTextMessageReceived
 *
 *****************************************************************************/
void Downloads::onTextMessageReceived(const QString& message)
{
  QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());

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

    info.chapterInfo.processChapter(entry["chapter"].toObject());
  }
  emit queueUpdated();
}

/******************************************************************************
 *
 * Method: clear()
 *
 *****************************************************************************/
void Downloads::clear()
{
  NetworkManager::instance().get("downloads/clear");
}

/******************************************************************************
 *
 * Method: cancel()
 *
 *****************************************************************************/
void Downloads::cancel(qint32 index)
{
  NetworkManager::instance().deleteResource(
      QStringLiteral("download/%1/chapter/%2")
        .arg(_queue[index].mangaId)
        .arg(_queue[index].chapterIndex));
}
