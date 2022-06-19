import QtQuick
import QtQuick.Controls
import QtQuick.Window

import Tachidesk.Models

import "../../libs/QmlBridgeForMaterialDesignIcons/Icon.js" as MdiFont

Item {
  id: base
  property alias mangaNumber: chapterModel.mangaNumber
  property alias chapter: chapterModel.chapterNumber
  property int lastReadPage: 0
  property bool positioned: false
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
    function onChapterLoaded(lastRead) {
      if (lastRead && !positioned) {
        lastReadPage = lastRead
        listView.positionViewAtIndex(lastRead, ListView.Beginning)
      }
    }
  }

  function imageLoaded() {
    if (lastReadPage && !positioned) {
      listView.positionViewAtIndex(lastReadPage, ListView.Beginning)
      positioned = true
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
    snapMode: ListView.NoSnap
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
    onMovementStarted: {
      headerBar.height = 0
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

    MouseArea {
      anchors.centerIn: parent
      height: parent.height / 2
      width: parent.width / 2
      onClicked: !headerBar.height ? headerBar.height = 55 : headerBar.height = 0
    }
  }

  ToolBar {
    id: headerBar
    //Material.foreground: rootWindow.Material.foreground
    background: Rectangle {
      anchors.fill: parent
      color: "#424242"
      opacity: 0.8
    }

    Text {
      anchors {
        left: parent.left
        top: parent.top
        bottom: parent.bottom
      }
      font.family: "Material Design Icons"
      font.pixelSize: 20
      verticalAlignment: Text.AlignVCenter
      horizontalAlignment: Text.AlignHCenter
      color: "#F5F5F5"
      text: MdiFont.Icon.arrowLeft
    }

    MouseArea {
      anchors {
        left: parent.left
        top: parent.top
        bottom: parent.bottom
      }
      width: 50
      onClicked: pop()
    }

    Text {
      id: chapterText
      anchors {
        margins: 5
        bottom: parent.bottom
        left: parent.left
        right: parent.right
        top: parent.top
      }
      color: "#F5F5F5"
      font.pixelSize: 14
      wrapMode: Text.WordWrap
      maximumLineCount: 3
      text: "Chapter %1".arg(chapterModel.chapterNumber)
      style: Text.Outline
      styleColor: "black"
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
    }
    height: 55
    visible: height > 0
    anchors {
      top: parent.top
      right: parent.right
      left: parent.left
    }
    Behavior on height {
      NumberAnimation {
          easing.type: Easing.OutCubic
      }
    }
  }
}
