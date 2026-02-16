#include "ChapterModel.h"
#include "../networkmanager.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <qcoreapplication.h>
#include <QQmlEngine>
#include <QStringBuilder>

/******************************************************************************
 *
 * ChapterModel
 *
 *****************************************************************************/
ChapterModel::ChapterModel(QObject* parent)
  : QAbstractListModel(parent)
{
}

/******************************************************************************
 *
 * classBegin
 *
 *****************************************************************************/
void ChapterModel::classBegin()
{
}

/******************************************************************************
 *
 * componentComplete
 *
 *****************************************************************************/
void ChapterModel::componentComplete()
{
  requestChapter(_chapterId);

  QJsonObject chaptersVariablesObj;
  QJsonObject conditionObj;
  conditionObj.insert("mangaId", _mangaNumber);
  chaptersVariablesObj.insert("condition", conditionObj);
  QJsonArray orderArray;
  {
    QJsonObject order1;
    order1.insert("by", "SOURCE_ORDER");
    order1.insert("byType", "ASC");
    orderArray.append(order1);
  }
  chaptersVariablesObj.insert("order", orderArray);

  NetworkManager::instance().postGraphQL(graphql::qtui::client::query::GET_CHAPTERS_ID::GetOperationName(), graphql::qtui::client::query::GET_CHAPTERS_ID::GetRequestText(), std::move(chaptersVariablesObj),
    [&](graphql::response::Value&& data) {
      auto parsed = graphql::qtui::client::query::GET_CHAPTERS_ID::parseResponse(std::move(data));
      _chapters = std::move(parsed);

      auto it = std::find_if(_chapters.chapters.nodes.begin(),
        _chapters.chapters.nodes.end(),
        [this](const auto& entry) {
          return entry.id == _chapterId;
        });
    });
}

/******************************************************************************
 *
 * Method: getChapterByRow()
 *
 *****************************************************************************/
const graphql::qtui::client::mutation::GET_CHAPTER_PAGES_FETCH::Response* ChapterModel::getChapterFetchByRow(
    quint32 index, quint32& chapterNumber) const
{
  for (const auto& chapter : _chaptersFetch) {
    if (index < chapter.fetchChapterPages->pages.size() + chapterNumber) {
      return &chapter;
    }
    chapterNumber += chapter.fetchChapterPages->pages.size();
  }
  return &_chaptersFetch.back();
}

/******************************************************************************
 *
 * Method: getLastChapter()
 *
 *****************************************************************************/
quint32 ChapterModel::getLastChapter() const {
  if (_chaptersFetch.empty()) {
    return 0;
  }
  return _chaptersFetch.back().fetchChapterPages->chapter.sourceOrder;
}

/******************************************************************************
 *
 * Method: rowCount()
 *
 *****************************************************************************/
int ChapterModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return 0;
  }
  qint32 pages = 0;
  for (auto& chapter : _chaptersFetch) {
    pages += chapter.fetchChapterPages->pages.size();
  }

  return pages;
}

/******************************************************************************
 *
 * Method: data()
 *
 *****************************************************************************/
QVariant ChapterModel::data(const QModelIndex &index, int role) const {
  if (!((index.isValid()) &&
       (index.row() >= 0) &&
       (index.row() < rowCount())) || _chaptersFetch.empty())
  {
    qDebug() << "Invalid index or empty chaptersFetch" << index;
    return {};
  }

  quint32 chapterNumber = 0;
  const auto entry = getChapterFetchByRow(index.row(), chapterNumber);
  if (!entry) {
    qDebug() << "entry not found";
    return {};
  }

  switch (role)
  {
    case RoleUrl:
      {
        {}
        // return entry->fetchChapterPages->chapter.
      }
    case RoleName:
      {
        return QString::fromStdString(entry->fetchChapterPages->chapter.name);
      }
    case RoleChapterNumber:
      {
        // return entry->chapterNumber;
      }
    case RoleRead:
      {
        // return entry->read;
      }
    case RoleIndex:
      {
        return entry->fetchChapterPages->chapter.sourceOrder;
      }
    case RolePageCount:
      {
        return entry->fetchChapterPages->chapter.pageCount;
      }
    case RoleChapterCount:
      {
        // return entry->chapterCount;
      }
    case RoleChapterUrl:
      {
        return NetworkManager::instance().resolvedPath().resolved(
            QString::fromStdString(entry->fetchChapterPages->pages[index.row() - chapterNumber]));
      }
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
                                          {RoleIndex,         "chapterIndex"},
                                          {RolePageCount,     "pageCount"},
                                          {RoleChapterUrl,    "chapterUrl"},
                                          {RoleChapterCount,  "chapterCount"},};

  return roles;
}

/******************************************************************************
 *
 * Method: get()
 *
 *****************************************************************************/
QVariantMap ChapterModel::get(int row) const
{
  QVariantMap map;
  QModelIndex modelIndex = index(row, 0);
  QHash<int, QByteArray> roles = roleNames();

  for (auto it = roles.begin(); it != roles.end(); ++it) {
    map.insert(it.value(), data(modelIndex, it.key()));
  }
  return map;
}


/******************************************************************************
 *
 * Method: lastPageRead()
 *
 *****************************************************************************/
void ChapterModel::updateChapter(qint32 page)
{
  if (_chaptersFetch.empty() || page < 0) {
    return;
  }

  quint32 chapterNumber = 0;
  const auto entry = getChapterFetchByRow(page, chapterNumber);
  if (!entry) {
    return;
  }
  _chapterName = QString::fromStdString(entry->fetchChapterPages->chapter.name);
  _pageCount = entry->fetchChapterPages->chapter.pageCount;
  _pageIndex = page - chapterNumber + 1;
  pageCountChanged();
  pageIndexChanged();
  chapterNumberChanged();

  // NetworkManager::instance().patch("lastPageRead", page - chapterNumber,
  //     QStringLiteral("manga/%1/chapter/%2").arg(_mangaNumber).arg(entry->index));
}

/******************************************************************************
 *
 * Method: requestNext()
 *
 *****************************************************************************/
void ChapterModel::requestNext(bool forward)
{
  if (_chaptersFetch.empty() || _requestingChapter) {
    return;
  }

  auto chapterId = !forward
      ? _chaptersFetch.begin()->fetchChapterPages->chapter.id
      : _chaptersFetch.back().fetchChapterPages->chapter.id;
  auto it = std::find_if(
      _chapters.chapters.nodes.begin(), _chapters.chapters.nodes.end(),
      [chapterId=chapterId](const auto &entry) {
        return entry.id == chapterId;
      });

  if (it == _chapters.chapters.nodes.end()) {
    qDebug() << "Chapter not found in chapters list";
    return;
  }

  auto index = std::distance(_chapters.chapters.nodes.begin(), it);
  if ((index >= _chapters.chapters.nodes.size() - 1 && forward) ||
      (index == 0 && !forward)) {
    qDebug() << "No more chapters to fetch" << index;
    return;
  }
  qint32 nextChapter =
      _chapters.chapters.nodes[forward ? index + 1 : index - 1].id;
  requestChapter(nextChapter, forward);
}

/******************************************************************************
 *
 * Method: lastPageRead()
 *
 *****************************************************************************/
void ChapterModel::requestChapter(qint32 chapter, bool forward)
{
  if (_requestingChapter) {
    return;
  }

  for (auto& c : _chaptersFetch) {
    if (c.fetchChapterPages->chapter.id == chapter) {
      qDebug() << "Chapter already loaded:" << chapter;
      return;
    }
  }

  QJsonObject variablesObj;

  QJsonObject input;
  input.insert("chapterId", chapter);
  variablesObj.insert("input", input);

  _requestingChapter = true;
  emit requestingChapterChanged();

  NetworkManager::instance().postGraphQL(graphql::qtui::client::mutation::GET_CHAPTER_PAGES_FETCH::GetOperationName(), graphql::qtui::client::mutation::GET_CHAPTER_PAGES_FETCH::GetRequestText(), std::move(variablesObj),
    [&, forward=forward](graphql::response::Value&& data) {
    qint32 pageStart = 0;
    qint32 pageEnd = 0;
    if (forward) {
      for (auto& chapter : _chaptersFetch) {
        pageStart += chapter.fetchChapterPages->chapter.pageCount;
      }
    }
    auto parsed = graphql::qtui::client::mutation::GET_CHAPTER_PAGES_FETCH::parseResponse(std::move(data));
    _pageCount = parsed.fetchChapterPages->chapter.pageCount;
    _chapterName = QString::fromStdString(parsed.fetchChapterPages->chapter.name);
    _chapterId = parsed.fetchChapterPages->chapter.id;
    _chapterCount = parsed.fetchChapterPages->chapter.sourceOrder;

    pageEnd = parsed.fetchChapterPages->chapter.pageCount + pageStart - 1;

    emit chapterIdChanged();
    emit chapterNameChanged();
    emit pageCountChanged();

    if (_chaptersFetch.empty()) {
      beginResetModel();
      _chaptersFetch.emplace_back(parsed);
      endResetModel();
    }
    else {
      beginInsertRows({}, pageStart, pageEnd);

      auto it = std::lower_bound(_chaptersFetch.begin(), _chaptersFetch.end(), parsed, [](const auto& a, const auto& b) {
        return a.fetchChapterPages->chapter.id < b.fetchChapterPages->chapter.id;
      });
      qDebug() << "Inserting chapter at index" << std::distance(_chaptersFetch.begin(), it)
               << "for chapter" << parsed.fetchChapterPages->chapter.id << "pages start and end:" << pageStart << pageEnd;
      _chaptersFetch.emplace(it, parsed);

      endInsertRows();
    }

    _requestingChapter = false;
    emit requestingChapterChanged();
  });
}


