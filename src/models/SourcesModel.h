#pragma once

#include <QAbstractListModel>
#include <QQmlParserStatus>

#include <qqml.h>

class SourcesModel : public QAbstractListModel, public QQmlParserStatus
{
  Q_OBJECT
  QML_ELEMENT
  Q_INTERFACES(QQmlParserStatus)

  struct SourceInfo {
    QString  id;
    QString  name;
    QString  lang;
    QString  iconUrl;
    bool     supportsLatest;
    bool     isConfigurable;
  };
  std::vector<SourceInfo> _sources;

protected:

  void classBegin() override;

  void componentComplete() override;

  virtual QHash<int, QByteArray> roleNames() const override;

public:

  enum Role {
    RoleIconUrl = Qt::UserRole + 1,
    RoleName,
    RoleLang,
    RoleId,
    RoleSupportsLatest,
    RoleConfigurable,
  };

  SourcesModel(QObject* parent = nullptr);

  virtual int rowCount(
     const QModelIndex &parent = QModelIndex()) const override;

  virtual QVariant data(
     const QModelIndex &index,
     int role = Qt::DisplayRole) const override;

  void receivedReply(const QJsonDocument& reply);

  Q_INVOKABLE void refresh();
};
