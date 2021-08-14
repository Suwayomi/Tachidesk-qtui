#include "apibase.h"

ApiBase::ApiBase()
{

}

ApiBase::ApiBase(QSettings* settings, QNetworkAccessManager* man, QObject* parent) :
  QObject(parent), _settings(settings), _man(man)
{

}