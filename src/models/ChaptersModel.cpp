#include "ChaptersModel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <qcoreapplication.h>
#include <QQmlEngine>

#define exportQmlType(ns, cls) qmlRegisterType<cls>(#ns, 1, 0, #cls)
#define IMPLEMENT_QTQUICK_TYPE(ns, cls) \
  void register ## cls ## Type() { exportQmlType(ns, cls); } \
  Q_COREAPP_STARTUP_FUNCTION(register ## cls ## Type)

IMPLEMENT_QTQUICK_TYPE(Tachidesk.Models, ChaptersModel)

/******************************************************************************
 *
 * ChaptersModel
 *
 *****************************************************************************/
ChaptersModel::ChaptersModel(QObject* parent)
  : QAbstractListModel(parent)
{
}

void ChaptersModel::classBegin() { }

/******************************************************************************
 *
 * componentComplete
 *
 *****************************************************************************/
void ChaptersModel::componentComplete()
{
  _networkManager->get(QStringLiteral("manga/%1/chapters").arg(_mangaNumber));
  disconnect(this, &ChaptersModel::networkManagerChanged, this, nullptr);

  connect(
      _networkManager,
      &NetworkManager::recievedReply,
      this,
      &ChaptersModel::recievedReply);
}

/******************************************************************************
 *
 * Method: recieveReply()
 *
 *****************************************************************************/
void ChaptersModel::recievedReply(const QJsonDocument& reply)
{
  beginResetModel();
  _chapters.clear();

  for (const auto& entry_arr : reply.array()) {
    const auto& entry = entry_arr.toObject();
    auto& info        = _chapters.emplace_back();
    info.url          = entry["url"].toInt();
    info.name         = entry["name"].toString();
    info.chapterNumber= entry["chapterNumber"].toInt();
    info.read         = entry["read"].toBool();
    info.index        = entry["index"].toInt();
    info.pageCount    = entry["pageCount"].toInt();
    info.chapterCount = entry["chapterCount"].toInt();
    //info. author artist genre status
  }

  endResetModel();
}

/******************************************************************************
 *
 * Method: rowCount()
 *
 *****************************************************************************/
int ChaptersModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return 0;
  }

  return _chapters.size();
}

/******************************************************************************
 *
 * Method: data()
 *
 *****************************************************************************/
QVariant ChaptersModel::data(const QModelIndex &index, int role) const {
  if (!((index.isValid()) &&
       (index.row() >= 0) &&
       (index.row() < rowCount())))
  {
    return {};
  }

  const auto& entry = _chapters[index.row()];

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
                  QStringLiteral(":%1/api/v1/manga/%2/chapter/%3/page/")
                    .arg(_networkManager->port()).arg(_mangaNumber).arg(entry.index);
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
QHash<int, QByteArray> ChaptersModel::roleNames() const {
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

