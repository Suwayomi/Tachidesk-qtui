#include "networkmanager.h"

#include <QAuthenticator>
#include <QDesktopServices>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkInterface>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QUrlQuery>

#include "settings.h"

/********************************************************************
 *
 *  NetworkManager()
 *
 ********************************************************************/
NetworkManager::NetworkManager(
  std::shared_ptr<Settings> &settings, const QString &host, QObject *parent)
: QObject(parent)
, QQmlNetworkAccessManagerFactory()
, _host(host.endsWith('/') ? host : host + '/')
, _settings(settings)
{
  _username = _settings->username();
  _password = _settings->password();

  connect(_settings.get(), &Settings::hostnameChanged, [&]() {
    _host = _settings->hostname();
  });

  connect(_settings.get(), &Settings::usernameChanged, [&]() {
    _username = _settings->username();
  });

  connect(_settings.get(), &Settings::passwordChanged, [&]() {
    _password = _settings->password();
  });

  _man = create(parent);
}

/********************************************************************
 *
 *  NetworkManager::instance()
 *
 ********************************************************************/
NetworkManager& NetworkManager::instance() {
  return *s_instance;
}

/********************************************************************
 *
 *  NetworkManager::create()
 *
 ********************************************************************/
QNetworkAccessManager *NetworkManager::create(QObject *parent)
{
  auto man   = new QNetworkAccessManager(parent);
  auto cache = new QNetworkDiskCache(parent);
  cache->setCacheDirectory(
    QStandardPaths::writableLocation(QStandardPaths::CacheLocation) +
    "/network-cache");
  cache->setMaximumCacheSize(419430400);

  man->setCache(cache);

  connect(
    man,
    &QNetworkAccessManager::authenticationRequired,
    [&](QNetworkReply *, QAuthenticator *aAuthenticator) {
      aAuthenticator->setUser(_username);
      aAuthenticator->setPassword(_password);
    });

  return man;
}

/********************************************************************
 *
 * handleNetworkError
 *
 ********************************************************************/
bool handleNetworkError(QNetworkReply *reply)
{
  if (reply->error() != QNetworkReply::NoError) {
    if (reply->error() >= 1 && reply->error() <= 199) {
      // no connection
    }

    qDebug() << reply->errorString() << reply->error();

    return false;
  }

  return true;
}

/********************************************************************
 *
 *  get()
 *
 ********************************************************************/
void NetworkManager::get(const QString &endpoint)
{
  getEndpoint(endpoint, &NetworkManager::endpointReply);
}

/********************************************************************
 *
 *  endpointReply()
 *
 ********************************************************************/
void NetworkManager::endpointReply() { emit receivedReply(processReply()); }

/********************************************************************
 *
 *  get()
 *
 ********************************************************************/
void NetworkManager::get(const QUrl& uri, QObject* context, const Callback& callback)
{
  qDebug().noquote().nospace()
     << "NetworkManager: get " << _host.resolved(QString("api/v1/")).resolved(uri);

  QNetworkRequest req(_host.resolved(QString("api/v1/")).resolved(uri));

  auto* reply = _man->get(req);

  connect(reply, &QNetworkReply::finished, context,
    [=,this]()
  {

    switch (reply->error()) {
      case QNetworkReply::AuthenticationRequiredError:
        break;

      default:
        break;
    }

    if (reply->error() != QNetworkReply::NoError) {
      qDebug().noquote().nospace()
          << "NetworkManager: get: " << reply->errorString() << reply->error();
      return;
    }

    QJsonParseError parser;
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &parser);

    if (parser.error != QJsonParseError::NoError) {
      qDebug().noquote().nospace()
          << "NetworkManager: get: Error: "
          << parser.errorString() << parser.error;
      return;
    }

    callback(doc);
  });

  connect(reply, &QNetworkReply::finished, reply,
   &QNetworkReply::deleteLater);
}

/********************************************************************
 *
 *  post()
 *
 ********************************************************************/
void NetworkManager::post(const QString &endpoint, const QUrlQuery &query)
{
  QNetworkRequest request;
  request.setUrl(_host.resolved(QString("api/v1/")).resolved(endpoint));
  request.setAttribute(
    QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);

  QByteArray dataParam;

  // request.setRawHeader("Authorization", "Basic " +
  // QByteArray(QString("%1:%2").arg(_username).arg(_password)).toBase64());
  _man->post(request, dataParam.append(query.toString().toStdString().c_str()));
}

/********************************************************************
 *
 *  processReply()
 *
 ********************************************************************/
QJsonDocument NetworkManager::processReply()
{
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (!handleNetworkError(reply)) {
    return QJsonDocument();
  }
  QByteArray data = reply->readAll();

  QJsonParseError error;
  QJsonDocument doc = QJsonDocument::fromJson(data, &error);

  reply->deleteLater();
  return doc;
}

/********************************************************************
 *
 *  get()
 *
 ********************************************************************/
void NetworkManager::get(
  const QString &endpoint,
  const std::function<void(const QJsonDocument &)> &func)
{
  QNetworkRequest request;
  request.setUrl(_host.resolved(QString("api/v1/")).resolved(endpoint));
  request.setAttribute(
    QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);

  QNetworkReply *reply = _man->get(request);

  connect(reply, &QNetworkReply::finished, this, [&]() {
    QNetworkReply *r = qobject_cast<QNetworkReply *>(sender());
    if (!handleNetworkError(r)) {
      return;
    }
    QByteArray data = r->readAll();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

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
void NetworkManager::deleteResource(const QString &endpoint)
{
  QNetworkRequest request;
  request.setUrl(_host.resolved(QString("api/v1/")).resolved(endpoint));
  request.setAttribute(
    QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);

  QNetworkReply *reply = _man->deleteResource(request);

  connect(
    reply, &QNetworkReply::finished, this, &NetworkManager::endpointReply);
}

/********************************************************************
 *
 *  patchReply()
 *
 ********************************************************************/
void NetworkManager::patchReply()
{
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (!handleNetworkError(reply)) {
    return;
  }

  reply->deleteLater();

  emit receivePatch();
}
