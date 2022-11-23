#include "MangaDetailsModel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <qcoreapplication.h>
#include <QQmlEngine>
#include <QDesktopServices>
#include <QStringBuilder>

#include "../networkmanager.h"

/******************************************************************************
 *
 * MangaDetailsModel
 *
 *****************************************************************************/
MangaDetailsModel::MangaDetailsModel(QObject* parent)
  : QObject(parent)
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
 * componentComplete
 *
 *****************************************************************************/
void MangaDetailsModel::componentComplete()
{
  NetworkManager::instance().get(QUrl(u"manga"_qs % '/' % QString::number(_mangaNumber)), this,
      [&](const auto& doc) {
        _data = doc.object().toVariantMap();

        emit loaded();
        emit dataChanged();
      });
}

/******************************************************************************
 *
 * Method: addToLibrary()
 *
 *****************************************************************************/
void MangaDetailsModel::addToLibrary()
{
  NetworkManager::instance().get(QStringLiteral("manga/%1/library").arg(_mangaNumber));

  _data["inLibrary"] = true;
  emit dataChanged();
}

/******************************************************************************
 *
 * Method: removeToLibrary()
 *
 *****************************************************************************/
void MangaDetailsModel::removeFromLibrary()
{
  NetworkManager::instance().deleteResource(QStringLiteral("manga/%1/library").arg(_mangaNumber));

  _data["inLibrary"] = false;
  emit dataChanged();
}

/******************************************************************************
 *
 * Method: openUrl()
 *
 *****************************************************************************/
void MangaDetailsModel::openUrl()
{
  QDesktopServices::openUrl(_data["realUrl"].toUrl());
}
