#pragma once
#include "graphql/tachideskClient.h"
#include <QAbstractListModel>
#include <QQmlParserStatus>
#include <qqml.h>

class LibraryModel : public QAbstractListModel, public QQmlParserStatus
{
  Q_OBJECT
  QML_ELEMENT
  Q_INTERFACES(QQmlParserStatus)

  graphql::qtui::client::query::GET_CATEGORY_MANGAS::Response _entries;

protected:
  virtual QHash<int, QByteArray> roleNames() const override;

  void classBegin() override;

  void componentComplete() override;

public:

  enum Role {
    RoleTitle = Qt::UserRole + 1,
    RoleThumbnail,
    RoleId,
    RoleUnread,
    RoleRecentlyRead,
  };


  LibraryModel(QObject* parent = nullptr);

  virtual int rowCount(
     const QModelIndex &parent = QModelIndex()) const override;

  virtual QVariant data(
     const QModelIndex &index,
     int role = Qt::DisplayRole) const override;

  Q_INVOKABLE void refreshLibrary();

public slots:
};
