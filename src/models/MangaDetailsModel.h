#pragma once

#include <QAbstractListModel>
#include <QQmlParserStatus>
#include <qqml.h>

class MangaDetailsModel : public QObject, public QQmlParserStatus
{
  Q_OBJECT
  QML_ELEMENT
  Q_INTERFACES(QQmlParserStatus)

  Q_PROPERTY(qint32 mangaNumber MEMBER _mangaNumber NOTIFY mangaNumberChanged)
  Q_PROPERTY(QVariantMap data  MEMBER _data NOTIFY dataChanged)

  qint32 _mangaNumber;
  QVariantMap _data;

protected:

  void classBegin() override;

  void componentComplete() override;

public:

  MangaDetailsModel(QObject* parent = nullptr);

  Q_INVOKABLE void addToLibrary();
  Q_INVOKABLE void removeFromLibrary();
  Q_INVOKABLE void openUrl();
signals:
   void mangaNumberChanged();
   void dataChanged();
   void loaded();
};
