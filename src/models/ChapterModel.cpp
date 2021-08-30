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

/******************************************************************************
 *
 * classBegin
 *
 *****************************************************************************/
void ChapterModel::classBegin()
{
  gotChapter = [&](const QJsonDocument& reply)
  {
    auto insert_sorted = [&](auto& vec, const ChapterInfo& item) {
      vec.insert(std::upper_bound(vec.begin(), vec.end(), item,
            [](const auto& a, const auto&b){ return a.index < b.index; }), item);
    };

    qint32 pageStart = 0;
    qint32 pageEnd = 0;
    for (auto& chapter : _chapters) {
      pageStart += chapter.pageCount;
    }

    const auto& entry = reply.object();
    ChapterInfo info;
    info.url          = entry["url"].toString();
    info.name         = entry["name"].toString();
    info.uploadDate   = entry["uploadDate"].toInt();
    info.chapterNumber= entry["chapterNumber"].toInt();
    info.read         = entry["read"].toBool();
    info.index        = entry["index"].toInt();
    info.pageCount    = entry["pageCount"].toInt();
    info.chapterCount = entry["chapterCount"].toInt();

    _chapterCount = info.chapterCount;

    pageEnd = info.pageCount + pageStart - 1;

    beginInsertRows({}, pageStart, pageEnd);

    insert_sorted(_chapters, info);

    endInsertRows();
  };
}

/******************************************************************************
 *
 * componentComplete
 *
 *****************************************************************************/
void ChapterModel::componentComplete()
{
  requestChapter(_chapterNumber);
}

/******************************************************************************
 *
 * Method: getChapterByRow()
 *
 *****************************************************************************/
const ChapterModel::ChapterInfo* ChapterModel::getChapterByRow(const QModelIndex& index, quint32& chapterNumber) const
{
  for (const auto& chapter : _chapters) {
    if (index.row() < chapter.pageCount + chapterNumber) {
      return &chapter;
    }
    chapterNumber += chapter.pageCount;
  }
  qDebug() << "not found!" << index.row();
  return nullptr;
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
  for (auto& chapter : _chapters) {
    pages += chapter.pageCount;
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
       (index.row() < rowCount())))
  {
    return {};
  }

  quint32 chapterNumber = 0;
  const auto entry = getChapterByRow(index, chapterNumber);
  if (!entry) {
    qDebug() << "entry not found";
    return {};
  }

  switch (role)
  {
    case RoleUrl:
      {
        return entry->url;
      }
    case RoleName:
      {
        return entry->name;
      }
    case RoleChapterNumber:
      {
        return entry->chapterNumber;
      }
    case RoleRead:
      {
        return entry->read;
      }
    case RoleIndex:
      {
        return entry->index;
      }
    case RolePageCount:
      {
        return entry->pageCount;
      }
    case RoleChapterCount:
      {
        return entry->chapterCount;
      }
    case RoleChapterUrl:
      {
        return _networkManager->hostname() +
                  QStringLiteral(":%1/api/v1/manga/%2/chapter/%3/page/%4")
                    .arg(_networkManager->port()).arg(_mangaNumber).arg(entry->index).arg(index.row() - chapterNumber);
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

/******************************************************************************
 *
 * Method: lastPageRead()
 *
 *****************************************************************************/
void ChapterModel::updateChapter(qint32 page, bool read)
{
  _networkManager->patch("lastPageRead", page, "read", read ? "true" : "false",
      QStringLiteral("manga/%1/chapter/%2").arg(_mangaNumber).arg(_chapterNumber));
}

/******************************************************************************
 *
 * Method: lastPageRead()
 *
 *****************************************************************************/
void ChapterModel::requestChapter(quint32 chapter)
{
  if (chapter > _chapterCount) {
    return;
  }
  for (auto& c: _chapters) {
    if (c.index == chapter) {
      return;
    }
  }
  _networkManager->get(QStringLiteral("manga/%1/chapter/%2").arg(_mangaNumber).arg(chapter), gotChapter);
}
