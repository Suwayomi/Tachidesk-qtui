import QtQuick
import QtQuick.Controls
import QtQuick.Window

import Tachidesk.Models

Item {
  id: base
  property alias mangaNumber: chapterModel.mangaNumber
  property alias chapter: chapterModel.chapterNumber
  property int lastReadPage: 0
  signal chapterRead(int mangaId, int chapter)

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

  Connections {
    target: chapterModel
    // doesn't work?
    function onChapterLoaded(lastRead) {
      if (lastRead) {
        lastReadPage = lastRead
        listView.positionViewAtIndex(lastRead, ListView.Beginning)
      }
    }
  }

  function imageLoaded() {
    if (lastReadPage) {
      listView.positionViewAtIndex(lastReadPage, ListView.Beginning)
      lastReadPage = 0
    }
  }

  PinchArea {
    id: pinchArea
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
    contentWidth:  pinchArea.width
    synchronousDrag: true
    delegate: WebtoonImage {
      onImageLoaded: base.imageLoaded()
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
      currentIndex = indexIs
    }
    onAtYEndChanged: {
      if (!listviewLoaded) {
        return
      }
      if (listView.atYEnd) {
        chapterRead(chapterModel.mangaNumber, chapterModel.chapterNumber)
        chapterModel.chapterNumber++
        chapterModel.requestChapter(chapterModel.chapterNumber)
      }
    }

    Text {
      id: infoText
      anchors {
        margins: 5
        bottom: parent.bottom
        left: parent.left
        right: parent.right
      }
      color: "#F5F5F5"
      font.pixelSize: 14
      wrapMode: Text.WordWrap
      maximumLineCount: 3
      text: ("%1 / %2").arg(chapterModel.pageIndex).arg(chapterModel.pageCount)
      style: Text.Outline
      styleColor: "black"
      horizontalAlignment: Text.AlignHCenter
    }
  }

}
