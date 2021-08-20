#include "ChapterModel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <qcoreapplication.h>
#include <QQmlEngine>

#define exportQmlType(ns, cls) qmlRegisterType<cls>(#ns, 1, 0, #cls)
#define IMPLEMENT_QTQUICK_TYPE(ns, cls) \
  void register ## cls ## Type() { exportQmlType(ns, cls); } \
  Q_COREAPP_STARTUP_FUNCTION(register ## cls ## Type)

IMPLEMENT_QTQUICK_TYPE(Tachidesk.Models, ChapterModel)

/******************************************************************************
 *
 * ChapterModel
 *
 *****************************************************************************/
ChapterModel::ChapterModel(QObject* parent)
  : QAbstractListModel(parent)
{
}

void ChapterModel::classBegin() { }

/******************************************************************************
 *
 * componentComplete
 *
 *****************************************************************************/
void ChapterModel::componentComplete()
{
  _networkManager->get(QStringLiteral("manga/%1/chapter/%2").arg(_mangaNumber).arg(_chapterNumber));

  connect(
      _networkManager,
      &NetworkManager::recievedReply,
      this,
      &ChapterModel::recievedReply);
}

/******************************************************************************
 *
 * Method: recieveReply()
 *
 *****************************************************************************/
void ChapterModel::recievedReply(const QJsonDocument& reply)
{
  if (reply.isObject()) {
    disconnect(_networkManager, &NetworkManager::recievedReply, this, nullptr);
  } else {
    return;
  }

  beginResetModel();
  _chapters.reset();

  const auto& entry = reply.object();
  auto& info        = _chapters.emplace();
  info.url          = entry["url"].toString();
  info.name         = entry["name"].toString();
  info.uploadDate   = entry["uploadDate"].toInt();
  info.chapterNumber= entry["chapterNumber"].toInt();
  info.read         = entry["read"].toBool();
  info.index        = entry["index"].toInt();
  info.pageCount    = entry["pageCount"].toInt();
  info.chapterCount = entry["chapterCount"].toInt();
  //info. author artist genre status

  endResetModel();
}

/******************************************************************************
 *
 * Method: rowCount()
 *
 *****************************************************************************/
int ChapterModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid() && _chapters.has_value()) {
    return 0;
  }

  return _chapters->pageCount;
}

/******************************************************************************
 *
 * Method: data()
 *
 *****************************************************************************/
QVariant ChapterModel::data(const QModelIndex &index, int role) const {
  if (!((index.isValid()) &&
       (index.row() >= 0) &&
       (index.row() < rowCount())) ||
      !_chapters.has_value())
  {
    return {};
  }

  const auto& entry = *_chapters;

  switch (role)
  {
    case RoleUrl:
      {
        return entry.url;
      }
    case RoleName:
      {
        return entry.name;
      }
    case RoleChapterNumber:
      {
        return entry.chapterNumber;
      }
    case RoleRead:
      {
        return entry.read;
      }
    case RoleIndex:
      {
        return entry.index;
      }
    case RolePageCount:
      {
        return entry.pageCount;
      }
    case RoleChapterCount:
      {
        return entry.chapterCount;
      }
    case RoleChapterUrl:
      {
        return _networkManager->hostname() +
                  QStringLiteral(":%1/api/v1/manga/%2/chapter/%3/page/%4")
                    .arg(_networkManager->port()).arg(_mangaNumber).arg(entry.index).arg(index.row() + 1);
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
QHash<int, QByteArray> ChapterModel::roleNames() const {
  static QHash<int, QByteArray> roles = { {RoleUrl,           "url"},
                                          {RoleName,          "name"},
                                          {RoleChapterNumber, "chapterNumber"},
                                          {RoleRead,          "read"},
                                          {RoleIndex,         "index"},
                                          {RolePageCount,     "pageCount"},
                                          {RoleChapterUrl,    "chapterUrl"},
                                          {RoleChapterCount,  "chapterCount"},};

  return roles;
}
