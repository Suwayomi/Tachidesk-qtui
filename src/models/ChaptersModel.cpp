#include "ChaptersModel.h"
#include "DownloadsModel.h"
#include "../networkmanager.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <qcoreapplication.h>
#include <QQmlEngine>

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
 * receivePatchReply
 *
 *****************************************************************************/
void ChaptersModel::receivePatchReply()
{
}

/******************************************************************************
 *
 * requestChapters
 *
 *****************************************************************************/
void ChaptersModel::requestChapters(bool onlineFetch)
{
  _loading = true;
  emit loadingChanged();

  NetworkManager::instance().get(QUrl(u"manga"_qs % '/' % QString::number(_mangaNumber) % u"/chapters/?onlineFetch=" % (onlineFetch ? "true" : "false") ), this,
    [&](const auto& doc)
  {
    if (doc.isEmpty()) {
      return;
    }

    bool reset = static_cast<quint32>(doc.array().size()) != _chapters.size();
    if (reset) {
      beginResetModel();
    }

    _chapters.clear();

    for (const auto& entry_arr : doc.array()) {
      const auto& entry = entry_arr.toObject();
      auto& info        = _chapters.emplace_back();
      info.processChapter(entry);
      if (!info.read) {
        _lastReadChapter = info.index;
      }
    }
    emit lastReadChapterChanged();

    if (reset) {
      endResetModel();
    }
    else {
      emit dataChanged(createIndex(0, 0), createIndex(_chapters.size(), 0));
    }

    if (!_cachedChapters && _autoUpdate) {
      _cachedChapters = true;
      requestChapters(true);
    }
    else {
      _loading = false;
      emit loadingChanged();
    }
  });
}

/******************************************************************************
 *
 * componentComplete
 *
 *****************************************************************************/
void ChaptersModel::componentComplete()
{
  connect(
      &NetworkManager::instance(),
      &NetworkManager::receivePatch,
      this,
      &ChaptersModel::receivePatchReply);

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
void ChaptersModel::chapterRead(quint64 chapter, bool read)
{
  auto c = std::lower_bound(_chapters.rbegin(), _chapters.rend(), chapter,
      [](const auto& c, quint64 value) {
        return c.index < value;
      });
  if (c == _chapters.rend()) {
    return;
  }

  c->read = read;
  assert(chapter == c->index);

  NetworkManager::instance().patch("read", read ? "true" : "false",
      QStringLiteral("manga/%1/chapter/%2").arg(_mangaNumber).arg(c->index));

  auto index = (_chapters.rend() - c - 1);
  emit dataChanged(createIndex(index, 0), createIndex(index, 0), { RoleRead });

  emit lastReadChapterChanged();
}

/******************************************************************************
 *
 * Method: chapterRead()
 *
 *****************************************************************************/
void ChaptersModel::previousChaptersRead(quint32 chapter, bool read)
{
  NetworkManager::instance().patch("markPrevRead", read ? "true" : "false",
      QStringLiteral("manga/%1/chapter/%2").arg(_mangaNumber).arg(chapter));

  qint32 start = 0, end = 0;
  for (auto& c : _chapters) {
    if (c.index < chapter) {
      c.read = read;
      if (!start) {
        start = end;
      }
    }
    end++;
  }

  emit dataChanged(createIndex(start, 0), createIndex(end, 0), { RoleRead });
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
    _downloads->setupWebsocket();
    connect(_downloads.get(), &DownloadsModel::downloadsUpdated, this, &ChaptersModel::onDownloadsUpdated);
  }

  auto downloadEndpoint = QStringLiteral("download/%1/chapter/%2");
  auto getChapter = [&](const auto& check) {
    for (const auto& chapter : _chapters) {
      if (check(chapter) || chapter.downloaded) {
        continue;
      }
      NetworkManager::instance().get(downloadEndpoint.arg(_mangaNumber).arg(chapter.index));
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
        NetworkManager::instance().get(downloadEndpoint.arg(_mangaNumber).arg(chapterIndex));
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
