#pragma once

#include <QAbstractListModel>
#include <QQmlParserStatus>

#include "common_structs.h"
#include "graphql/tachideskClient.h"
#include <qqml.h>

class ChapterModel : public QAbstractListModel, public QQmlParserStatus
{
  Q_OBJECT
  QML_ELEMENT
  Q_INTERFACES(QQmlParserStatus)

  Q_PROPERTY(qint32 mangaNumber   MEMBER _mangaNumber    NOTIFY mangaNumberChanged)
  Q_PROPERTY(qint32 chapterNumber MEMBER _chapterNumber  NOTIFY chapterNumberChanged)
  Q_PROPERTY(QString chapterName  MEMBER _chapterName    NOTIFY chapterNameChanged)
  Q_PROPERTY(qint32 pageCount     MEMBER _pageCount      NOTIFY pageCountChanged)
  Q_PROPERTY(qint32 pageIndex     MEMBER _pageIndex      NOTIFY pageIndexChanged)
  Q_PROPERTY(qint32 chapterId     MEMBER _chapterId      NOTIFY chapterIdChanged)
  Q_PROPERTY(bool requestingChapter MEMBER _requestingChapter NOTIFY requestingChapterChanged)

  std::vector<graphql::client::mutation::GET_CHAPTER_PAGES_FETCH::Response> _chaptersFetch;

  qint32 _mangaNumber;
  qint32 _chapterNumber;
  QString _chapterName;
  quint32 _chapterCount;
  qint32 _pageCount = 0;
  qint32 _pageIndex = 0;
  qint32 _chapterId = 0;
  bool _requestingChapter = false;

  const graphql::client::mutation::GET_CHAPTER_PAGES_FETCH::Response* getChapterFetchByRow(quint32 index, quint32& chapterNumber) const;
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

  Q_INVOKABLE void updateChapter(qint32 page);
  Q_INVOKABLE void requestChapter(qint32 chapter);
  Q_INVOKABLE QVariantMap get(int row) const;
  Q_INVOKABLE quint32 getLastChapter() const;
signals:
   void mangaNumberChanged();
   void pageCountChanged();
   void pageIndexChanged();
   void chapterNumberChanged();
   void chapterNameChanged();
   void chapterIdChanged();
   void chapterLoaded(int lastRead);
   void requestingChapterChanged();

};
