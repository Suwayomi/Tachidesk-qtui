#include <QNetworkRequest>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDesktopServices>
//#include <QNetworkConfiguration>
//#include <QNetworkConfigurationManager>
#include <QNetworkInterface>
#include <QAuthenticator>
#include <QEventLoop>
#include <QUrlQuery>
#include <QStandardPaths>

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
    , QQmlNetworkAccessManagerFactory()
    , _host(host)
    , _settings(settings)
{
  man = new QNetworkAccessManager(this);
  _cache = new QNetworkDiskCache(this);
  _cache->setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/network-cache");
  man->setCache(_cache);

  _username = _settings->username();
  _password = _settings->password();

  connect(
      _settings.get(),
      &Settings::hostnameChanged,
      [&]() {
        _host = _settings->hostname();
      });

  connect(
     _settings.get(),
     &Settings::usernameChanged,
     [&]() {
       _username = _settings->username();
    });

  connect(
     _settings.get(),
     &Settings::passwordChanged,
     [&]() {
       _password = _settings->password();
    });

  connect(man, &QNetworkAccessManager::authenticationRequired,
    [&](QNetworkReply *,
        QAuthenticator *aAuthenticator)
  {
    qDebug() << "using username and password: " << _username << _password;
    aAuthenticator->setUser(_username);
    aAuthenticator->setPassword(_password);
  });

}

QNetworkAccessManager* NetworkManager::create(QObject* parent)
{
  return man;
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

/********************************************************************
 *
 *  get()
 *
 ********************************************************************/
void NetworkManager::getUpdates(const QString& endpoint)
{
  getEndpoint(endpoint, &NetworkManager::updatesReply);
}

/********************************************************************
 *
 *  post()
 *
 ********************************************************************/
void NetworkManager::post(const QString& endpoint, const QUrlQuery& query)
{
  QUrl url(resolvedPath().arg("/api/v1/" + endpoint));

  QNetworkRequest request;
  request.setUrl(url);
  request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferNetwork);

  QByteArray dataParam;

  //request.setRawHeader("Authorization", "Basic " + QByteArray(QString("%1:%2").arg(_username).arg(_password)).toBase64());
  man->post(request, dataParam.append(query.toString().toStdString().c_str()));
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
  request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferNetwork);

  QNetworkReply *reply = man->get(request);

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
  request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferNetwork);

  QNetworkReply *reply = man->deleteResource(request);

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
  emit receivedReply(processReply());
}

/********************************************************************
 *
 *  userReply()
 *
 ********************************************************************/
void NetworkManager::chaptersReply()
{
  emit receiveChapters(processReply());
}

/********************************************************************
 *
 *  userReply()
 *
 ********************************************************************/
void NetworkManager::updatesReply()
{
  emit receiveUpdates(processReply());
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

  emit receivePatch();
}
