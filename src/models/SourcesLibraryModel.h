#pragma once

#include <QAbstractListModel>
#include <QQmlParserStatus>

#include <optional>
#include <qqml.h>

class SourcesLibraryModel : public QAbstractListModel, public QQmlParserStatus
{
  Q_OBJECT
  QML_ELEMENT
  Q_INTERFACES(QQmlParserStatus)

  Q_PROPERTY(QString source MEMBER _source NOTIFY sourceChanged)

  struct SourceDetails {
    QString  artist;
    QString  author;
    QString  description;
    QString  genre;
    QString  status;
  };

  struct SourceInfo {
    qint32   id;
    QString  title;
    QString  thumbnailUrl;
    QString  url;
    bool     isInitialized;
    bool     inLibrary;
    bool     freshData;

    std::optional<SourceDetails> details;
  };
  std::vector<SourceInfo> _sources;

  QString _source;
  qint32 _pageNumber = 0;

  bool resetModel = true;

protected:

  void classBegin() override;

  void componentComplete() override;

  virtual QHash<int, QByteArray> roleNames() const override;

public:

  enum Role {
    RoleThumbnailUrl = Qt::UserRole + 1,
    RoleTitle,
    RoleId,
    RoleUrl,
    RoleInitialized,
    RoleInLibrary,
    RoleFreshData,
  };

  SourcesLibraryModel(QObject* parent = nullptr);

  virtual int rowCount(
     const QModelIndex &parent = QModelIndex()) const override;

  virtual QVariant data(
     const QModelIndex &index,
     int role = Qt::DisplayRole) const override;

  std::function<void (const QJsonDocument& reply)> receiveReply;

  Q_INVOKABLE void search(const QString& searchTerm);
  Q_INVOKABLE void next();
signals:
  void sourceChanged();
};
