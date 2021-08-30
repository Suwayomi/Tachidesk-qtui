import QtQuick 2.8
import QtQuick.Controls 2.15

import Tachidesk.Models 1.0

Item {
  id: base
  property alias mangaNumber: chapterModel.mangaNumber
  property alias chapter: chapterModel.chapterNumber
  property bool read: false
  signal chapterRead(int chapter)

  ChapterModel {
    id: chapterModel
    nm: networkManager
  }

  ListView {
    id: listView
    anchors.fill: parent
    model: chapterModel
    cacheBuffer: 10000
    maximumFlickVelocity: 100000
    delegate: WebtoonImage {}
    // debug rectangle
    //delegate: Rectangle {
    //  width: base.width
    //  height: 500
    //  Text {
    //    font.pixelSize: 30
    //    width: base.width
    //    wrapMode: Text.Wrap
    //    text: name +  chapterUrl
    //  }
    //}
    property bool listviewLoaded: false
    // TODO position at last page read
    Component.onCompleted: {
      listviewLoaded = true
    }
    onMovementEnded: {
      var indexIs = indexAt(contentX,contentY + base.height - 10)
      if (indexIs == count - 1) {
        read = true
        chapterRead(chapter)
      }
      chapterModel.updateChapter(indexIs, read)
    }
    onAtYEndChanged: {
      if (!listviewLoaded) {
        console.log("not done")
        return
      }
      if (listView.atYEnd) {
        chapterModel.chapterNumber++
        chapterModel.requestChapter(chapterModel.chapterNumber)
      }
    }
  }
}
