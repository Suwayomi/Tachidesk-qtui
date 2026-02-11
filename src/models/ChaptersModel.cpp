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

  QJsonObject variablesObj;
  QJsonObject conditionObj;
  conditionObj.insert("mangaId", _mangaNumber);
  variablesObj.insert("condition", conditionObj);
  QJsonArray orderArray;
  {
    QJsonObject order1;
    order1.insert("by", "SOURCE_ORDER");
    order1.insert("byType", "DESC");
    orderArray.append(order1);
  }
  variablesObj.insert("order", orderArray);

  NetworkManager::instance().postGraphQL(graphql::qtui::client::query::GET_CHAPTERS_MANGA::GetOperationName(), std::move(variablesObj),
    [&](graphql::response::Value&& data) {
      auto parsed = graphql::qtui::client::query::GET_CHAPTERS_MANGA::parseResponse(std::move(data));
      beginResetModel();
      _chapters = std::move(parsed);
      endResetModel();

      _loading = false;
      emit loadingChanged();
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

  return _chapters.chapters.nodes.size();
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

  const auto& entry = _chapters.chapters.nodes[index.row()];

  switch (role)
  {
    case RoleUrl:
      {
        return QString::fromStdString(entry.realUrl.value_or(""));
      }
    case RoleName:
      {
        return QString::fromStdString(entry.name);
      }
    case RoleChapterNumber:
      {
        return entry.chapterNumber;
      }
    case RoleRead:
      {
        return entry.isRead;
      }
    case RoleChapterIndex:
      {
        return entry.sourceOrder;
      }
    case RoleDownloaded:
      {
        return entry.isDownloaded;
      }
    case RoleDownloadProgress:
      {
        //return entry.downloadProgress.value_or(-1);
      }
    case RoleChapterId:
      {
        return entry.id;
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
  static QHash<int, QByteArray> roles = {
      {RoleUrl, "url"},
      {RoleName, "name"},
      {RoleChapterNumber, "chapterNumber"},
      {RoleRead, "read"},
      {RoleChapterIndex, "chapterIndex"},
      {RoleChapterId, "chapterId"},
      {RolePageCount, "pageCount"},
      {RoleDownloaded, "downloaded"},
      {RoleDownloadProgress, "progress"},
      {RoleLastPageRead, "lastPageRead"},
      {RoleChapterCount, "chapterCount"},
  };

  return roles;
}

/******************************************************************************
 *
 * Method: chapterRead()
 *
 *****************************************************************************/
void ChaptersModel::chapterRead(qint32 chapterId, bool read)
{
  QJsonObject variablesObj;
  variablesObj.insert("chapterIdsToDelete", QJsonArray());
  variablesObj.insert("deleteChapters", false);
  variablesObj.insert("getBookmarked", false);
  variablesObj.insert("getLastPageRead", true);
  variablesObj.insert("getRead", true);
  QJsonObject input;
  QJsonArray idsArray;
  idsArray.append(chapterId);
  input.insert("ids", idsArray);
  QJsonObject patchObj;
  patchObj.insert("isRead", read);
  patchObj.insert("lastPageRead", 0);
  input.insert("patch", patchObj);
  variablesObj.insert("input", input);
  variablesObj.insert("mangaId", -1);
  variablesObj.insert("trackProgress", false);

  NetworkManager::instance().postGraphQL(graphql::qtui::client::mutation::UPDATE_CHAPTERS::GetOperationName(), std::move(variablesObj),
    [&](graphql::response::Value&& data) {
      auto parsed = graphql::qtui::client::mutation::UPDATE_LIBRARY::parseResponse(std::move(data));
      auto it = std::find_if(_chapters.chapters.nodes.begin(),
                             _chapters.chapters.nodes.end(),
                             [&chapterId](const auto &entry) {
                               return entry.id == chapterId;
                             });
      if (it != _chapters.chapters.nodes.end()) {
        it->isRead = true;
      }

      size_t index = std::distance(_chapters.chapters.nodes.begin(), it);
      emit dataChanged(createIndex(index, 0), createIndex(index, 0), {RoleRead});
  });
}

/******************************************************************************
 *
 * Method: previousChaptersRead()
 *
 *****************************************************************************/
void ChaptersModel::previousChaptersRead(quint32 chapter, bool read)
{
  QJsonObject variablesObj;
  variablesObj.insert("chapterIdsToDelete", QJsonArray());
  variablesObj.insert("deleteChapters", false);
  variablesObj.insert("getBookmarked", false);
  variablesObj.insert("getLastPageRead", true);
  variablesObj.insert("getRead", true);
  QJsonObject input;
  QJsonArray idsArray;
  for (const auto& c : _chapters.chapters.nodes) {
    if (c.sourceOrder < chapter) {
      idsArray.append(c.id);
    }
  }
  input.insert("ids", idsArray);
  QJsonObject patchObj;
  patchObj.insert("isRead", read);
  patchObj.insert("lastPageRead", 0);
  input.insert("patch", patchObj);
  variablesObj.insert("input", input);
  variablesObj.insert("mangaId", -1);
  variablesObj.insert("trackProgress", false);

  NetworkManager::instance().postGraphQL(graphql::qtui::client::mutation::UPDATE_CHAPTERS::GetOperationName(), std::move(variablesObj),
    [&](graphql::response::Value&& data) {
      auto parsed = graphql::qtui::client::mutation::UPDATE_CHAPTERS::parseResponse(std::move(data));

      for (const auto& chapter : parsed.updateChapters->chapters) {
        auto it = std::find_if(_chapters.chapters.nodes.begin(), _chapters.chapters.nodes.end(),
          [&chapter](const auto &entry) {
            return entry.id == chapter.id;
            });
        if (it != _chapters.chapters.nodes.end()) {
          it->isRead = chapter.isRead;
        }
      }

      emit dataChanged(createIndex(0, 0), createIndex(_chapters.chapters.nodes.size() - 1, 0), {RoleRead});
  });
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
    for (const auto& chapter : _chapters.chapters.nodes) {
      if (check(chapter) || chapter.isDownloaded) {
        continue;
      }
      // NetworkManager::instance().get(downloadEndpoint.arg(_mangaNumber).arg(chapter.index));
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
        getChapter([&](const auto& chapter){ return chapter.isRead; });
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
    for (auto& chapter : _chapters.chapters.nodes) {
      if (info.mangaId == _mangaNumber &&
          info.chapterInfo.chapterNumber == chapter.chapterNumber)
      {
        //chapter.downloadProgress = info.progress;
        //chapter.downloaded = info.progress >= 100;
        emit dataChanged(createIndex(row, 0), createIndex(row, 0), { RoleDownloadProgress, RoleDownloaded });
        break;
      }
      row++;
    }
  }
}
