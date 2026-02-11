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

#include "graphql/tachideskClient.h"

#include <graphqlservice/GraphQLResponse.h>
#include <graphqlservice/GraphQLParse.h>
#include <graphqlservice/JSONResponse.h>

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
    const auto& h = _settings->hostname();
    _host = h.endsWith('/') ? h : h + '/';
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
 *  postGraphQL()
 *
 ********************************************************************/
void NetworkManager::postGraphQL(
    const std::string &query, QJsonObject &&value,
    std::function<void(graphql::response::Value &&)> callback) {
  QNetworkRequest request;
  request.setRawHeader("Content-Type", "application/json");
  request.setUrl(_host.resolved(QString("api/graphql/")));

  QJsonObject requestObject;
  requestObject.insert("query", QString::fromStdString(graphql::qtui::client::GetRequestText()));
  requestObject.insert("operationName", QString::fromStdString(query));
  requestObject["variables"] = value;

  QJsonDocument doc(requestObject);
  QByteArray jsonData = doc.toJson();

  request.setRawHeader("Authorization", "Basic " +
    QByteArray(QString("%1:%2").arg(_username).arg(_password).toStdString()).toBase64());

  auto reply = _man->post(request, jsonData);
  if (!reply) {
    qDebug() << "no reply";
    return;
  }
  connect(reply,  &QNetworkReply::finished, this,
     [=]()-> void
  {
    switch (reply->error()) {
      case QNetworkReply::AuthenticationRequiredError:
      case QNetworkReply::ContentAccessDenied:
        qDebug() << "there was error : " << reply->error();
        return;

      default:
        break;
    }

    if (reply->error() != QNetworkReply::NoError) {
      qDebug() << "Errors" << reply->error() << reply->errorString();
      return;
    }

    QByteArray responseData = reply->readAll();

    try {
      // Parse JSON to graphql::response::Value
      graphql::response::Value gqlResponse =
          graphql::response::parseJSON(responseData.toStdString());
      auto members = gqlResponse.release<graphql::response::MapType>();

      auto it = std::find_if(
          members.begin(), members.end(),
          [](const auto& pair) { return pair.first == "data"; });

      if (it == members.end()) {
          throw std::runtime_error("Missing 'data' key in GraphQL response");
      }

      graphql::response::Value data = std::move(it->second);

      callback(std::move(data));
    }
    catch (const std::exception& ex) {
      qDebug() << responseData;
      qWarning() << "Failed to parse GraphQL response:" << ex.what();
    }
  });

  connect( reply,  &QNetworkReply::finished,  reply,
    &QNetworkReply::deleteLater);
}

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
