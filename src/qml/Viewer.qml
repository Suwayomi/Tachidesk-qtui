import QtQuick
import QtQuick.Controls
import QtQuick.Window

import Tachidesk.Models

import "../../libs/QmlBridgeForMaterialDesignIcons/Icon.js" as MdiFont

Item {
  id: viewerBase
  property alias mangaNumber: chapterModel.mangaNumber
  property alias chapter: chapterModel.chapterNumber
  property int lastReadPage: 0
  property bool positioned: false
  signal chapterRead(int mangaId, int chapter)

  Component.onCompleted: {
    if (Qt.platform.os === "android")
      changeWindowVisibility(Window.FullScreen)

    settings.viewer == 0 ?
      listView.sourceComponent = webtoonViewer :
      listView.sourceComponent = mangaViewer
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
        console.log("jumping to:", lastRead)
        //listView.setCurrentIndex(lastRead)
      }
    }
  }

  function imageLoaded() {
    if (lastReadPage && !positioned) {
      listView.setCurrentIndex(lastReadPage)
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


  Component {
    id: mangaViewer
    MangaViewer {
      anchors.fill:parent
      model: chapterModel
    }
  }

  Component {
    id: webtoonViewer
    WebtoonViewer {
      anchors.fill:parent
      model: chapterModel
    }
  }

  states: [
    State {
      name: "MangaReader"
      when: settings.viewer == 1//Settings.MANGA
      PropertyChanges {
        target: listView
        sourceComponent: mangaViewer
      }
    },
    State {
      name: "WebtoonReader"
      when: settings.viewer == 0//Settings.WEBTOON
      PropertyChanges {
        target: listView
        sourceComponent: webtoonViewer
      }
    }
  ]

  Loader {
    id: listView
    anchors.fill: parent
  }

  Text {
    id: infoText
    anchors {
      margins: 5
      bottom: listView.bottom
      left: listView.left
      right: listView.right
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

  ViewerPopup {
    id: settingsPopup
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
      text: "%1".arg(chapterModel.chapterName)
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

    Text {
      anchors {
        right: parent.right
        top: parent.top
        bottom: parent.bottom
      }
      font.family: "Material Design Icons"
      font.pixelSize: 20
      verticalAlignment: Text.AlignVCenter
      horizontalAlignment: Text.AlignHCenter
      color: "#F5F5F5"
      text: MdiFont.Icon.dotsHorizontal
    }

    MouseArea {
      anchors {
        right: parent.right
        top: parent.top
        bottom: parent.bottom
      }
      width: 50
      onClicked: settingsPopup.open()
    }

  }

}
