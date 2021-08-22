import QtQuick 2.8
import QtQuick.Controls 2.15

import Tachidesk.Models 1.0

Item {
  id: base
  property alias mangaNumber: chapterModel.mangaNumber
  property alias chapter: chapterModel.chapterNumber
  property bool read: false

  ChapterModel {
    id: chapterModel
    nm: networkManager
  }

  ListView {
    id: listView
    anchors.fill: parent
    model: chapterModel
    cacheBuffer: 10000
    delegate: Loader {
      source: "WebtoonImage.qml"
    }
    onMovementEnded: {
      var indexIs = indexAt(contentX,contentY + base.height - 10)
      if (indexIs == count - 1) {
        read = true
      }
      chapterModel.updateChapter(indexIs, read)
    }
  }
}
