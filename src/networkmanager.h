#pragma once

#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QNetworkReply>
#include <QQmlNetworkAccessManagerFactory>
#include <QSslSocket>
#include <QTimer>
#include <memory>

class Settings;

class NetworkManager : public QObject, public QQmlNetworkAccessManagerFactory
{
  Q_OBJECT

  QString _host;
  QString _username;
  QString _password;

  QNetworkDiskCache *_cache;
  QNetworkAccessManager *man;

  std::shared_ptr<Settings> _settings;

signals:
  void receivedReply(const QJsonDocument &reply);
  void receiveChapters(const QJsonDocument &reply);
  void receiveUpdates(const QJsonDocument &reply);
  void receiveSources(const QJsonDocument &reply);
  void receiveExtensions(const QJsonDocument &reply);
  void abortChapterRequest();
  void receivePatch();

private slots:
  void endpointReply();
  void chaptersReply();
  void updatesReply();
  void sourcesReply();
  void extensionsReply();
  void patchReply();

public:
  NetworkManager(
    std::shared_ptr<Settings> &settings,
    const QString &host,
    QObject *parent = 0);

  ~NetworkManager() = default;

  QNetworkAccessManager *create(QObject *parent) override;

  void get(const QString &endpoint);
  void get(
    const QString &endpoint,
    const std::function<void(const QJsonDocument &)> &func);
  void getChapters(const QString &endpoint);
  void getUpdates(const QString &endpoint);
  void getSources(const QString &endpoint);
  void getExtensions(const QString &endpoint);
  void deleteResource(const QString &endpoint);
  // TODO needs similar to patch for extra arguments
  void post(const QString &endpoint, const QUrlQuery &query);

  auto resolvedPath() const { return QStringLiteral("%1%2").arg(_host); }

  void patch(QHttpMultiPart *patchData, const QString &endpoint)
  {
    QNetworkRequest request;
    request.setUrl(QUrl(resolvedPath().arg("/api/v1/" + endpoint)));
    request.setAttribute(
      QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);

    auto reply = man->sendCustomRequest(request, "PATCH", patchData);
    patchData->setParent(reply);

    connect(reply, &QNetworkReply::finished, this, &NetworkManager::patchReply);
  }

  template <typename First, typename Second, typename... Args>
  void patch(
    QHttpMultiPart *patchData,
    const First &first,
    const Second &second,
    Args... args)
  {
    QHttpPart part;
    part.setHeader(
      QNetworkRequest::ContentDispositionHeader,
      QVariant(QStringLiteral("form-data; name=\"%1\"").arg(first)));
    part.setBody(QByteArray(QStringLiteral("%1").arg(second).toUtf8()));

    patchData->append(part);
    patch(patchData, args...);
  }

  template <typename... Args>
  void patch(Args... args)
  {
    auto patchData = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    patch(patchData, args...);
  }

  void abortRequest() { emit abortChapterRequest(); }

private:
  template <typename F>
  void getEndpoint(const QString &endpoint, F f)
  {
    QNetworkRequest request;
    request.setUrl(QUrl(resolvedPath().arg("/api/v1/" + endpoint)));
    request.setAttribute(
      QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);

    QNetworkReply *reply = man->get(request);

    connect(reply, &QNetworkReply::finished, this, f);

    connect(
      this, &NetworkManager::abortChapterRequest, reply, &QNetworkReply::abort);
  }
  QJsonDocument processReply();
};
