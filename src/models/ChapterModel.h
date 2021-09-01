#pragma once

#include <QAbstractListModel>
#include <QQmlParserStatus>

#include "networkmanager.h"

class ChapterModel : public QAbstractListModel, public QQmlParserStatus
{
  Q_OBJECT

  Q_PROPERTY(NetworkManager* nm READ getNetworkManager WRITE setNetworkManager NOTIFY networkManagerChanged)
  Q_PROPERTY(qint32 mangaNumber MEMBER _mangaNumber NOTIFY mangaNumberChanged)
  Q_PROPERTY(qint32 chapterNumber MEMBER _chapterNumber NOTIFY chapterNumberChanged)

  NetworkManager* _networkManager = nullptr;

  struct ChapterInfo {
    QString  url;
    QString  name;
    qint64   uploadDate;
    qint32   chapterNumber;
    bool     read;
    quint32  index;
    quint32  pageCount;
    quint32  chapterCount;
  };
  std::vector<ChapterInfo> _chapters;

  std::function<void (const QJsonDocument& )> gotChapter;

  qint32 _mangaNumber;
  qint32 _chapterNumber;
  quint32 _chapterCount;

  const ChapterInfo* getChapterByRow(qint32 index, quint32& chapterNumber) const;
protected:

  void classBegin() override;

  void componentComplete() override;

  virtual QHash<int, QByteArray> roleNames() const override;

public:

  enum Role {
    RoleUrl = Qt::UserRole + 1,
    RoleName,
    RoleChapterNumber,
    RoleRead,
    RoleIndex,
    RolePageCount,
    RoleChapterCount,
    RoleChapterUrl,
  };

  ChapterModel(QObject* parent = nullptr);

  virtual int rowCount(
     const QModelIndex &parent = QModelIndex()) const override;

  virtual QVariant data(
     const QModelIndex &index,
     int role = Qt::DisplayRole) const override;

  auto getNetworkManager() const
  {
    return _networkManager;
  }

  void setNetworkManager(NetworkManager* nm) {
    _networkManager = nm;
    networkManagerChanged();
  }

  Q_INVOKABLE void updateChapter(qint32 page);
  Q_INVOKABLE void requestChapter(quint32 chapter);
signals:
   void networkManagerChanged();
   void mangaNumberChanged();
   void chapterNumberChanged();
};
