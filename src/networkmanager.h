#pragma once

#include <QSslSocket>
#include <QNetworkAccessManager>
#include <QTimer>
#include <QHttpMultiPart>
#include <QNetworkReply>

#include <memory>

class Settings;

class NetworkManager : public QObject
{
  Q_OBJECT

  QString   _host;

  QNetworkAccessManager man;
  bool connectionOK = false;

  std::shared_ptr<Settings> _settings;

signals:
  void recievedReply(const QJsonDocument& reply);

private slots:
  void endpointReply();
  void patchReply();

public:
  NetworkManager(
      std::shared_ptr<Settings>& settings,
      const QString& host,
      QObject* parent = 0);

  ~NetworkManager() = default;

  void get(const QString& endpoint);
  void get(const QString& endpoint, const std::function<void(const QJsonDocument&)>& func);
  void deleteResource(const QString& endpoint);

  auto resolvedPath() const {
    return QStringLiteral("%1%2").arg(_host);
  }

  void patch(QHttpMultiPart* patchData, const QString& endpoint) {
    QNetworkRequest request;
    request.setUrl(QUrl(resolvedPath().arg("/api/v1/" + endpoint)));

    auto reply = man.sendCustomRequest(request, "PATCH", patchData);
    patchData->setParent(reply);

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        &NetworkManager::endpointReply);

  }

  template<typename First, typename Second, typename... Args>
  void patch(QHttpMultiPart* patchData, const First& first, const Second& second, Args... args) {
    QHttpPart part;
    part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QStringLiteral("form-data; name=\"%1\"").arg(first)));
    part.setBody(QByteArray(QStringLiteral("%1").arg(second).toUtf8()));

    patchData->append(part);
    patch(patchData, args...);
  }

  template<typename... Args>
  void patch(Args... args) {
    auto patchData = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    patch(patchData, args...);
  }

private:
  bool handleNetworkError(QNetworkReply* reply);
};
