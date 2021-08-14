#pragma once

#include <QString>
#include <QObject>

#include <QNetworkAccessManager>
#include <QSettings>

#define DECLARE_API_BASE(x) \
  public: x(QSettings* settings, QNetworkAccessManager* man, QObject* partent=0);

class ApiBase : public QObject {
  Q_OBJECT

protected:
  // handled by networkmanager
  QSettings* _settings;
  QNetworkAccessManager* _man;

public:
  ApiBase();
  ApiBase(QSettings* settings, QNetworkAccessManager* man, QObject* parent = 0);
};