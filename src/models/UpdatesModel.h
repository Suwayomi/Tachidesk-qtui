#pragma once

#include <QAbstractListModel>
#include <QQmlParserStatus>
#include <QWebSocket>
#include <QTimer>

#include "ChaptersModel.h"
#include "graphql/tachideskClient.h"

class DownloadsModel;
struct QueueInfo;

class UpdatesModel : public QAbstractListModel, public QQmlParserStatus
{
  Q_OBJECT
  QML_ELEMENT
  Q_INTERFACES(QQmlParserStatus)

  Q_PROPERTY(bool running MEMBER _running NOTIFY runningChanged)
  Q_PROPERTY(qint32 total MEMBER _total NOTIFY totalChanged)
  Q_PROPERTY(qint32 complete MEMBER _complete NOTIFY completeChanged)

  QWebSocket _webSocket;
  QTimer _updateStatusTimer;
  std::shared_ptr<DownloadsModel> downloads;
  std::map<quint32, std::shared_ptr<QueueInfo>> _queueInfo;
  graphql::client::query::GET_CHAPTERS_UPDATES::Response _entries;
  bool _isRequesting = false;

  bool _running = false;
  qint32 _total = 0;
  qint32 _complete = 0;
  qint32 _pageNumber = 0;
  bool _hasNext = false;

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
    RoleChapterUrl,
    RoleName,
    RoleChapterNumber,
    RoleRead,
    RoleChapterIndex,
    RoleChapterId,
    RolePageCount,
    RoleChapterCount,
    RoleLastPageRead,
    RoleDownloaded,
    RoleFetchedAt,
    RoleDownloadProgress,
    RoleDownloadPrepairing,
  };

  UpdatesModel(QObject* parent = nullptr);

  virtual int rowCount(
     const QModelIndex &parent = QModelIndex()) const override;

  virtual QVariant data(
     const QModelIndex &index,
     int role = Qt::DisplayRole) const override;

  Q_INVOKABLE void next();
  Q_INVOKABLE void pageRefresh();
  Q_INVOKABLE void refresh();
  Q_INVOKABLE void downloadChapter(int index);
  Q_INVOKABLE void chapterRead(qint32 Id, int chapter);
signals:
  void runningChanged();
  void totalChanged();
  void completeChanged();

public slots:
  void onConnected();
  void closed();
  void onTextMessageReceived(const QString& message);
  void onDownloadsUpdated(const std::vector<QueueInfo>& info);

private slots:
  void handleLibraryUpdateStatus(const QJsonObject& statusObj);
  void processChapterUpdateProgress(const QJsonObject& download);
  void requestUpdateStatus();
};
