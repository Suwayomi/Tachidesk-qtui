#pragma once

#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QNetworkReply>
#include <QQmlNetworkAccessManagerFactory>
#include <QSslSocket>
#include <QTimer>
#include <QJSEngine>
#include <QQmlEngine>
#include <memory>

class Settings;

class NetworkManager : public QObject, public QQmlNetworkAccessManagerFactory {
  Q_OBJECT

  private: inline static NetworkManager* s_instance = nullptr;
  private: bool _initialized = [this]() { s_instance = this; return true; }();
  public: static NetworkManager* create(QQmlEngine*, QJSEngine*) {
    QJSEngine::setObjectOwnership(s_instance, QJSEngine::CppOwnership);
    return s_instance; }

public:
  using Callback = std::function<void(const QJsonDocument&)>;

private:
  QUrl _host;
  QString _username;
  QString _password;

  QNetworkDiskCache *_cache;
  QNetworkAccessManager *man;

  std::shared_ptr<Settings> _settings;

signals:
  void receivedReply(const QJsonDocument &reply);
  void abortChapterRequest();
  void receivePatch();

private slots:
  void endpointReply();
  void patchReply();

public:
  NetworkManager(std::shared_ptr<Settings> &settings, const QString &host,
                 QObject *parent = 0);

  ~NetworkManager() = default;

  static NetworkManager& instance();

  QNetworkAccessManager *create(QObject *parent) override;

  const auto &username() const { return _username; }

  const auto &password() const { return _password; }

  void get(const QUrl& uri, QObject* context, const Callback& callback);
  void get(const QString &endpoint);
  void get(const QString &endpoint,
           const std::function<void(const QJsonDocument &)> &func);
  void deleteResource(const QString &endpoint);
  // TODO needs similar to patch for extra arguments
  void post(const QString &endpoint, const QUrlQuery &query);

  Q_INVOKABLE QUrl resolvedPath() const { return _host; }

  void patch(QHttpMultiPart *patchData, const QString &endpoint) {
    QNetworkRequest request;
    request.setUrl(_host.resolved(QString("api/v1/")).resolved(endpoint));
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
                         QNetworkRequest::PreferCache);

    auto reply = man->sendCustomRequest(request, "PATCH", patchData);
    patchData->setParent(reply);

    connect(reply, &QNetworkReply::finished, this, &NetworkManager::patchReply);
  }

  template <typename First, typename Second, typename... Args>
  void patch(QHttpMultiPart *patchData, const First &first,
             const Second &second, Args... args) {
    QHttpPart part;
    part.setHeader(
        QNetworkRequest::ContentDispositionHeader,
        QVariant(QStringLiteral("form-data; name=\"%1\"").arg(first)));
    part.setBody(QByteArray(QStringLiteral("%1").arg(second).toUtf8()));

    patchData->append(part);
    patch(patchData, args...);
  }

  template <typename... Args> void patch(Args... args) {
    auto patchData = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    patch(patchData, args...);
  }

  void abortRequest() { emit abortChapterRequest(); }

private:
  template <typename F> void getEndpoint(const QString &endpoint, F f) {
    QNetworkRequest request;
    request.setUrl(_host.resolved(_host.path() + QString("api/v1/")).resolved(endpoint));
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
                         QNetworkRequest::PreferCache);

    QNetworkReply *reply = man->get(request);

    connect(reply, &QNetworkReply::finished, this, f);

    connect(this, &NetworkManager::abortChapterRequest, reply,
            &QNetworkReply::abort);
  }
  QJsonDocument processReply();
};
