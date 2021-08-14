#pragma once

#include <QSettings>
#include <QSslSocket>
#include <QNetworkAccessManager>
#include <QTimer>

#include <memory>

class NetworkManager : public QObject
{
  Q_OBJECT

  QSettings _settings;
  QString   _host;
  QString   _port;

  QNetworkAccessManager man;
  bool connectionOK = false;

  static const quint16 PORT;
  static const QString Kraken;

signals:
  void recievedReply(const QJsonDocument& reply);

private slots:
  void endpointReply();

public:
  NetworkManager(const QString& host, const QString& port, QObject* parent = 0);
  ~NetworkManager() = default;

  void get(const QString& endpoint);

  const QString& hostname() const {
    return _host;
  }

  const auto& port() const {
    return _port;
  }

private:
  bool handleNetworkError(QNetworkReply* reply);
};
