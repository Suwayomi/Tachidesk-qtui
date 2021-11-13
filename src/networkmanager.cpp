#include <QNetworkRequest>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDesktopServices>
#include <QNetworkConfiguration>
#include <QNetworkConfigurationManager>
#include <QNetworkInterface>
#include <QEventLoop>
#include <QUrlQuery>

#include "networkmanager.h"
#include "settings.h"

/********************************************************************
 *
 *  NetworkManager()
 *
 ********************************************************************/
NetworkManager::NetworkManager(
  std::shared_ptr<Settings>& settings,
  const QString& host,
  QObject* parent)
    : QObject(parent)
    , _host(host)
    , _settings(settings)
{
  connect(
      _settings.get(),
      &Settings::hostnameChanged,
      [&]() {
        _host = _settings->hostname();
      });

}

/********************************************************************
 *
 * handleNetworkError
 *
 ********************************************************************/
bool handleNetworkError(QNetworkReply* reply)
{
  if (reply->error() != QNetworkReply::NoError)
  {
    if (reply->error() >= 1 && reply->error() <= 199)
    {
      // no connection
    }

    qDebug() << reply->errorString();

    return false;
  }

  return true;
}

/********************************************************************
 *
 *  get()
 *
 ********************************************************************/
void NetworkManager::get(const QString& endpoint)
{
  getEndpoint(endpoint, &NetworkManager::endpointReply);
}

/********************************************************************
 *
 *  get()
 *
 ********************************************************************/
void NetworkManager::getChapters(const QString& endpoint)
{
  getEndpoint(endpoint, &NetworkManager::chaptersReply);
}

void NetworkManager::post(const QString& endpoint, const QUrlQuery& query)
{
  QUrl url(resolvedPath().arg("/api/v1/" + endpoint));

  QNetworkRequest request;
  request.setUrl(url);

  QByteArray dataParam;

  man.post(request, dataParam.append(query.toString()));
}

/********************************************************************
 *
 *  processReply()
 *
 ********************************************************************/
QJsonDocument NetworkManager::processReply()
{
  QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());
  if (!handleNetworkError(reply)) {
      return QJsonDocument();
  }
  QByteArray data = reply->readAll();

  QJsonParseError error;
  QJsonDocument doc = QJsonDocument::fromJson(data,&error);

  reply->deleteLater();
  return doc;
}

/********************************************************************
 *
 *  get()
 *
 ********************************************************************/
void NetworkManager::get(
    const QString& endpoint,
    const std::function<void(const QJsonDocument&)>& func)
{
  QNetworkRequest request;
  request.setUrl(QUrl(resolvedPath().arg("/api/v1/" + endpoint)));

  QNetworkReply *reply = man.get(request);

  connect(
      reply,
      &QNetworkReply::finished,
      this,
      [&]() {
        QNetworkReply* r = qobject_cast<QNetworkReply *>(sender());
        if (!handleNetworkError(r)) {
            return;
        }
        QByteArray data = r->readAll();

        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(data,&error);

        if (func) {
          func(doc);
        }
        r->deleteLater();
      });
}

/********************************************************************
 *
 *  get()
 *
 ********************************************************************/
void NetworkManager::deleteResource(const QString& endpoint)
{
  QNetworkRequest request;
  request.setUrl(QUrl(resolvedPath().arg("/api/v1/" + endpoint)));

  QNetworkReply *reply = man.deleteResource(request);

  connect(
      reply,
      &QNetworkReply::finished,
      this,
      &NetworkManager::endpointReply);
}

/********************************************************************
 *
 *  userReply()
 *
 ********************************************************************/
void NetworkManager::endpointReply()
{
  emit recievedReply(processReply());
}


/********************************************************************
 *
 *  userReply()
 *
 ********************************************************************/
void NetworkManager::chaptersReply()
{
  emit recieveChapters(processReply());
}

/********************************************************************
 *
 *  patchReply()
 *
 ********************************************************************/
void NetworkManager::patchReply()
{
  QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());
  if (!handleNetworkError(reply)) {
      return;
  }

  reply->deleteLater();
}
