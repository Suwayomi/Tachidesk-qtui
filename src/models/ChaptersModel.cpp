#include "ChaptersModel.h"
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

IMPLEMENT_QTQUICK_TYPE(Tachidesk.Models, ChaptersModel)

void ChapterInfo::processChapter(const QJsonObject& entry)
{
  url          = entry["url"].toString();
  name         = entry["name"].toString();
  chapterNumber= entry["chapterNumber"].toInt();
  read         = entry["read"].toBool();
  index        = entry["index"].toInt();
  pageCount    = entry["pageCount"].toInt();
  chapterCount = entry["chapterCount"].toInt();
  lastPageRead = entry["lastPageRead"].toInt();
  downloaded   = entry["downloaded"].toBool();
  fetchedAt    = QDateTime::fromSecsSinceEpoch(entry["fetchedAt"].toInt());
}

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

/******************************************************************************
 *
 * gotChapters
 *
 *****************************************************************************/
void ChaptersModel::gotChapters(const QJsonDocument& reply)
{
  if (reply.isEmpty()) {
    return;
  }

  _loading = !_chapters.size();
  emit loadingChanged();

  _chapters.clear();

  beginResetModel();

  for (const auto& entry_arr : reply.array()) {
    const auto& entry = entry_arr.toObject();
    auto& info        = _chapters.emplace_back();
    info.processChapter(entry);
    if (!info.read) {
      _lastReadChapter = info.index;
    }
    //info. author artist genre status
  }
  emit lastReadChapterChanged();

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
      &NetworkManager::receiveChapters,
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

    case RoleDownloadProgress:
      {
        return entry.downloadProgress.value_or(-1);
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
                                          {RoleDownloadProgress, "progress"},
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

  _chapters[chapterIndex].read = !_chapters[chapterIndex].read;
  _networkManager->patch("read", _chapters[chapterIndex].read ? "true" : "false",
      QStringLiteral("manga/%1/chapter/%2").arg(_mangaNumber).arg(chapter));

  auto findLastReadChapter = [&]() {
    _lastReadChapter = 0;
    for (const auto& chapter : _chapters) {
      if (!chapter.read) {
        _lastReadChapter = chapter.index;
      }
    }
  };
  findLastReadChapter();
  emit lastReadChapterChanged();

  emit dataChanged(index(chapterIndex, 0), index(chapterIndex, 0));
}

/******************************************************************************
 *
 * Method: chapterRead()
 *
 *****************************************************************************/
void ChaptersModel::downloadChapter(qint32 downloadOption, qint32 chapterIndex)
{
  if (!_downloads) {
    _downloads = std::make_shared<DownloadsModel>();
    _downloads->setNetworkManager(_networkManager);
    _downloads->setupWebsocket();
    connect(_downloads.get(), &DownloadsModel::downloadsUpdated, this, &ChaptersModel::onDownloadsUpdated);
  }

  auto downloadEndpoint = QStringLiteral("download/%1/chapter/%2");
  auto getChapter = [&](const auto& check) {
    for (const auto& chapter : _chapters) {
      if (check(chapter) || chapter.downloaded) {
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
        _networkManager->get(downloadEndpoint.arg(_mangaNumber).arg(chapterIndex));
        break;
      }
  }
}

/******************************************************************************
 *
 * Method: onDownloadsUpdated()
 *
 *****************************************************************************/
void ChaptersModel::onDownloadsUpdated(const std::vector<QueueInfo>& queueInfo)
{
  for (auto& info : queueInfo) {
    int row = 0;
    for (auto& chapter : _chapters) {
      if (info.mangaId == _mangaNumber &&
          info.chapterInfo.chapterNumber == chapter.chapterNumber)
      {
        chapter.downloadProgress = info.progress;
        chapter.downloaded = info.progress >= 100;
        emit dataChanged(createIndex(row, 0), createIndex(row, 0), { RoleDownloadProgress, RoleDownloaded });
        break;
      }
      row++;
    }
  }
}
