#pragma once

#include <QtGlobal>
#include <QString>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>

struct ChapterInfo {
  QString  url;
  QString  name;
  qint32   chapterNumber;
  bool     read;
  bool     downloaded;
  quint32  index;
  quint32  pageCount;
  quint32  chapterCount;
  quint32  lastPageRead;
  QDateTime  fetchedAt;
  std::optional<qint32>  downloadProgress;
  std::optional<bool> downloadPrepairing;

  void processChapter(const QJsonObject& entry)
  {
    url          = entry["url"].toString();
    name         = entry["name"].toString();
    chapterNumber= entry["chapterNumber"].toInt();
    read         = entry["read"].toBool();
    index        = entry["index"].toInt();
    pageCount    = entry["pageCount"].toInt();
    chapterCount = entry["chapterCount"].toInt();
    lastPageRead = entry["lastPageRead"].toInt();
    downloaded   = entry["downloaded"].toBool();
    fetchedAt    = QDateTime::fromSecsSinceEpoch(entry["fetchedAt"].toInt());
  }
};

struct QueueInfo {
  quint32  chapterIndex;
  quint32  mangaId;
  QString  state;
  quint32  progress;
  quint32  tries;
  ChapterInfo chapterInfo;
};
