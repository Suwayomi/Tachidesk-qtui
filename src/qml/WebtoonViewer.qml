import QtQuick 2.8
import QtQuick.Controls 2.15
import QtQuick.Window 2.2

import Tachidesk.Models 1.0

Item {
  id: base
  property alias mangaNumber: chapterModel.mangaNumber
  property alias chapter: chapterModel.chapterNumber
  signal chapterRead(int chapter)

  Component.onCompleted: {
    if (Qt.platform.os === "android")
      changeWindowVisibility(Window.FullScreen)
  }
  Component.onDestruction: {
    if (Qt.platform.os === "android")
      changeWindowVisibility(Window.Windowed)
  }

  ChapterModel {
    id: chapterModel
    nm: networkManager
  }
  PinchArea {
    width: Math.max(listView.contentWidth, listView.width)
    height: Math.max(listView.contentHeight, listView.height)
    enabled: true
    pinch.target: listView
    pinch.minimumScale: 0.5
    pinch.maximumScale: 2
    pinch.dragAxis: Pinch.XAndYAxis
  }

  ListView {
    id: listView
    anchors.fill: parent
    model: chapterModel
    cacheBuffer: 10000
    maximumFlickVelocity: 10000
    delegate: WebtoonImage {
    }
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
      chapterModel.updateChapter(indexIs)
    }
    onAtYEndChanged: {
      if (!listviewLoaded) {
        return
      }
      if (listView.atYEnd) {
        chapterRead(chapterModel.chapterNumber)
        chapterModel.chapterNumber++
        chapterModel.requestChapter(chapterModel.chapterNumber)
      }
    }
  }
}
