#include "MangaDetailsModel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <qcoreapplication.h>
#include <QQmlEngine>
#include <QDesktopServices>

#define exportQmlType(ns, cls) qmlRegisterType<cls>(#ns, 1, 0, #cls)
#define IMPLEMENT_QTQUICK_TYPE(ns, cls) \
  void register ## cls ## Type() { exportQmlType(ns, cls); } \
  Q_COREAPP_STARTUP_FUNCTION(register ## cls ## Type)

IMPLEMENT_QTQUICK_TYPE(Tachidesk.Models, MangaDetailsModel)

void MangaDetails::processDetails(const QJsonObject& entry)
{
  id           = entry["id"].toInt();
  sourceId     = entry["sourceId"].toString();
  url          = entry["url"].toString();
  realUrl      = entry["realUrl"].toString();
  title        = entry["title"].toString();
  thumbnailUrl = entry["thumbnailUrl"].toString();
  initalized   = entry["intialized"].toBool();
  author       = entry["author"].toString();
  artist       = entry["artist"].toString();
  description  = entry["description"].toString();
  for (const auto& ge : entry["genre"].toArray()) {
    genre += ge.toString() + " ";
  }
  status       = entry["status"].toString();
  inLibrary    = entry["inLibrary"].toBool();

  const auto& source= entry["source"].toObject();
  sourceName   = source["name"].toString();
}

/******************************************************************************
 *
 * MangaDetailsModel
 *
 *****************************************************************************/
MangaDetailsModel::MangaDetailsModel(QObject* parent)
  : QAbstractListModel(parent)
{
}

/******************************************************************************
 *
 * classBegin()
 *
 *****************************************************************************/
void MangaDetailsModel::classBegin()
{
}

/******************************************************************************
 *
 * gotDetails
 *
 *****************************************************************************/
void MangaDetailsModel::gotDetails(const QJsonDocument& reply)
{
  disconnect(_networkManager, &NetworkManager::receivedReply, this, nullptr);

  beginResetModel();
  _entries.clear();


  const auto& entry = reply.object();
  auto& info        = _entries.emplace_back();
  info.processDetails(entry);

  endResetModel();

  emit loaded();
}

/******************************************************************************
 *
 * componentComplete
 *
 *****************************************************************************/
void MangaDetailsModel::componentComplete()
{
  connect(
      _networkManager,
      &NetworkManager::receivedReply,
      this,
      &MangaDetailsModel::gotDetails);

  _networkManager->get(QStringLiteral("manga/%1").arg(_mangaNumber));
}

/******************************************************************************
 *
 * Method: rowCount()
 *
 *****************************************************************************/
int MangaDetailsModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return 0;
  }

  return _entries.size();
}

/******************************************************************************
 *
 * Method: data()
 *
 *****************************************************************************/
QVariant MangaDetailsModel::data(const QModelIndex &index, int role) const {
  if (!((index.isValid()) &&
       (index.row() >= 0) &&
       (index.row() < rowCount())))
  {
    return {};
  }

  const auto& entry = _entries[index.row()];

  switch (role)
  {
    case RoleId:
      {
        return entry.id;
      }
    case RoleSourceId:
      {
        return entry.sourceId;
      }
    case RoleUrl:
      {
        return entry.url;
      }
    case RoleRealUrl:
      {
        return entry.realUrl;
      }
    case RoleTitle:
      {
        return entry.title;
      }
    case RoleThumbnailUrl:
      {
        return _networkManager->resolvedPath().arg(entry.thumbnailUrl);
      }
    case RoleInitialized:
      {
        return entry.initalized;
      }
    case RoleAuthor:
      {
        return entry.author;
      }
    case RoleArtist:
      {
        return entry.artist;
      }
    case RoleDescription:
      {
        return entry.description;
      }
    case RoleGenre:
      {
        return entry.genre;
      }
    case RoleStatus:
      {
        return entry.status;
      }
    case RoleInLibrary:
      {
        return entry.inLibrary;
      }
    case RoleSourceName:
      {
        return entry.sourceName;
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
QHash<int, QByteArray> MangaDetailsModel::roleNames() const {
  static QHash<int, QByteArray> roles = {
    {RoleUrl, "url"},
    {RoleRealUrl, "realUrl"},
    {RoleId, "id"},
    {RoleSourceId, "sourceId"},
    {RoleUrl, "url"},
    {RoleTitle, "title"},
    {RoleThumbnailUrl, "thumbnailUrl"},
    {RoleInitialized, "initialized"},
    {RoleAuthor, "author"},
    {RoleArtist, "artist"},
    {RoleDescription, "description"},
    {RoleGenre, "genre"},
    {RoleInLibrary, "inLibrary"},
    {RoleSourceName, "sourceName"},
    {RoleStatus, "status"}};

  return roles;
}

QVariantMap MangaDetailsModel::get(int row) const
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
 * Method: addToLibrary()
 *
 *****************************************************************************/
void MangaDetailsModel::addToLibrary()
{
  _networkManager->get(QStringLiteral("manga/%1/library").arg(_mangaNumber));

  _entries[0].inLibrary = true;
  emit dataChanged(index(0, 0), index(0, 0), {RoleInLibrary});
}

/******************************************************************************
 *
 * Method: removeToLibrary()
 *
 *****************************************************************************/
void MangaDetailsModel::removeFromLibrary()
{
  _networkManager->deleteResource(QStringLiteral("manga/%1/library").arg(_mangaNumber));

  _entries[0].inLibrary = false;
  emit dataChanged(index(0, 0), index(0, 0), {RoleInLibrary});
}

/******************************************************************************
 *
 * Method: openUrl()
 *
 *****************************************************************************/
void MangaDetailsModel::openUrl()
{
  QDesktopServices::openUrl(QUrl(_entries[0].realUrl));
}
