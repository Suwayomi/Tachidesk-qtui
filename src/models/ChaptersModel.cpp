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

/******************************************************************************
 *
 * ChaptersModel
 *
 *****************************************************************************/
ChaptersModel::~ChaptersModel()
{
  _networkManager->abortRequest();
}

/******************************************************************************
 *
 * requestChapters
 *
 *****************************************************************************/
void ChaptersModel::classBegin()
{
}

void ChaptersModel::gotChapters(const QJsonDocument& reply)
{
  if (reply.isEmpty()) {
    return;
  }

  beginResetModel();
  _chapters.clear();

  for (const auto& entry_arr : reply.array()) {
    const auto& entry = entry_arr.toObject();
    auto& info        = _chapters.emplace_back();
    info.url          = entry["url"].toString();
    info.name         = entry["name"].toString();
    info.chapterNumber= entry["chapterNumber"].toInt();
    info.read         = entry["read"].toBool();
    info.index        = entry["index"].toInt();
    info.pageCount    = entry["pageCount"].toInt();
    info.chapterCount = entry["chapterCount"].toInt();
    info.lastPageRead = entry["lastPageRead"].toInt();
    info.downloaded   = entry["downloaded"].toBool();
    //info. author artist genre status
  }

  endResetModel();

  if (!_cachedChapters) {
    _cachedChapters = true;
    requestChapters(true);
  }
}

/******************************************************************************
 *
 * requestChapters
 *
 *****************************************************************************/
void ChaptersModel::requestChapters(bool onlineFetch)
{
  _networkManager->getChapters(QStringLiteral("manga/%1/chapters/?onlineFetch=%2")
      .arg(_mangaNumber).arg(onlineFetch ? "true" : "false"));
}

/******************************************************************************
 *
 * componentComplete
 *
 *****************************************************************************/
void ChaptersModel::componentComplete()
{
  connect(
      _networkManager,
      &NetworkManager::recieveChapters,
      this,
      &ChaptersModel::gotChapters);

  requestChapters(_cachedChapters);
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
    case RoleChapterIndex:
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

    case RoleLastPageRead:
      {
        return entry.lastPageRead;
      }

    case RoleDownloaded:
      {
        return entry.downloaded;
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
                                          {RoleChapterIndex,  "chapterIndex"},
                                          {RolePageCount,     "pageCount"},
                                          {RoleDownloaded,    "downloaded"},
                                          {RoleLastPageRead,  "lastPageRead"},
                                          {RoleChapterCount,  "chapterCount"},};

  return roles;
}

/******************************************************************************
 *
 * Method: chapterRead()
 *
 *****************************************************************************/
void ChaptersModel::chapterRead(qint32 chapter)
{
  auto chapterIndex = _chapters.size() - chapter;
  _chapters[chapterIndex].read = true;
  emit dataChanged(index(chapterIndex, 0), index(chapterIndex, 0));
}

void ChaptersModel::downloadChapter(qint32 downloadOption)
{
  auto downloadEndpoint = QStringLiteral("download/%1/chapter/%2");
  auto getChapter = [&](const auto& check) {
    for (const auto& chapter : _chapters) {
      if (check(chapter)) {
        continue;
      }
      _networkManager->get(downloadEndpoint.arg(_mangaNumber).arg(chapter.index));
    }
  };

  switch (downloadOption) {
    case DownloadAll:
      {
        getChapter([&](const auto&){ return false; });
        break;
      }
    case DownloadUnread:
      {
        getChapter([&](const auto& chapter){ return chapter.read; });
        break;
      }
    case DownloadCustom:
      {
        break;
      }
  }
}
