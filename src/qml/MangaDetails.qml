import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import QtQuick.Window
import QtWebSockets

import Tachidesk.Qtui

import "../../libs/QmlBridgeForMaterialDesignIcons/Icon.js" as MdiFont

Item {
  id: base
  property alias mangaNumber: details.mangaNumber
  property bool doWrap: false
  property int chapterNumberPopup
  property bool chapterReadPopup: false

  signal mangaChanged();

  MangaDetailsModel {
    id: details
  }

  ChaptersModel {
    id: chaptersModel
    mangaNumber: base.mangaNumber
    autoUpdate: settings.autoUpdate
  }

  function markRead(mangaId, chapter) {
    chaptersModel.chapterRead(chapter, true)
  }

  Popup {
    id: popup
    width: 200
    height: 300
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnReleaseOutside
    anchors.centerIn: parent

    ColumnLayout {
      anchors.fill: parent
      Button {
        Layout.fillWidth: true
        Layout.fillHeight: true
        text: qsTr("Download All")
        onClicked: {
          chaptersModel.downloadChapter(ChaptersModel.DownloadAll)
          popup.close()
        }
      }
      Button {
        Layout.fillWidth: true
        Layout.fillHeight: true
        text: qsTr("Download Unread")
        onClicked: {
          chaptersModel.downloadChapter(ChaptersModel.DownloadUnread)
          popup.close()
        }
      }
    }
  }

  Popup {
    id: popupChapter
    width: base.width / 1.5
    height: 250
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    anchors.centerIn: parent
    ColumnLayout {
      anchors.fill: parent
      Button {
        Layout.fillWidth: true
        Layout.fillHeight: true
        text: qsTr("Download")
        onClicked: {
          chaptersModel.downloadChapter(ChaptersModel.DownloadCustom, chapterNumberPopup)
          popupChapter.close()
        }
      }
      Button {
        Layout.fillWidth: true
        Layout.fillHeight: true
        text: qsTr("Mark as read/unread")
        onClicked: {
          chaptersModel.chapterRead(chapterNumberPopup, !chapterReadPopup)
          popupChapter.close()
        }
      }
      Button {
        Layout.fillWidth: true
        Layout.fillHeight: true
        text: qsTr("Mark previous as read")
        onClicked: {
          chaptersModel.previousChaptersRead(chapterNumberPopup, true)
          popupChapter.close()
        }
      }
      Button {
        Layout.fillWidth: true
        Layout.fillHeight: true
        text: qsTr("Mark previous as unread")
        onClicked: {
          chaptersModel.previousChaptersRead(chapterNumberPopup, false)
          popupChapter.close()
        }
      }
    }
  }

  Image {
    id: backgroundImage
    source: networkManager.resolvedPath() + "api/v1/manga/%1/thumbnail".arg(mangaNumber)
    fillMode: Image.Tile
    width: detailsColumn.width
    anchors {
      top: parent.top
      bottom: parent.bottom
      //topMargin: (height / 1.2) - height
    }
  }

  Colorize {
    anchors.fill: backgroundImage
    source: backgroundImage
    lightness: 0.80
    saturation: 0.5
    hue: 0.57
  }

  Column {
    id: detailsColumn
    spacing: 8
    leftPadding: 4
    rightPadding: 4
    topPadding: 4
    width: parent.width
    height: {
      var heightLimit = parent.height * 3/8
      if (heightLimit < detailsColumn.childrenRect.height) {
        return heightLimit
      }
      return detailsColumn.childrenRect.height + 4 // 4 margin
    }

    RowLayout {
      width: parent.width

      Image {
        id: image
        source: networkManager.resolvedPath() + "api/v1/manga/%1/thumbnail".arg(mangaNumber)
        fillMode: Image.PreserveAspectFit
        sourceSize.width: parent.width * .33
      }
      ColumnLayout {
        Layout.fillWidth: true
        Text {
          Layout.fillWidth: true
          font.pixelSize: 20
          fontSizeMode: Text.Fit
          text: details.data.title
        }
        Text {
          Layout.fillWidth: true
          font.pixelSize: 20
          fontSizeMode: Text.Fit
          text: details.data.author
        }
        Text {
          Layout.fillWidth: true
          font.pixelSize: 20
          fontSizeMode: Text.Fit
          text: details.data.artist
        }
        Text {
          Layout.fillWidth: true
          fontSizeMode: Text.Fit
          font.pixelSize: 10
          text: "%1 - %2".arg(details.data.status).arg(details.data.source.name)
          color: "#222222"
        }
      }
    }

    Text {
      text: details.data.description
      elide: Text.ElideRight
      width: parent.width
      maximumLineCount: 2
      MouseArea {
        anchors.fill: parent
        onClicked: {
          if (!doWrap) {
            parent.wrapMode = Text.WordWrap
            parent.maximumLineCount = 40
            doWrap = true
          } else {
            parent.wrapMode = Text.NoWrap
            parent.maximumLineCount = 2
            doWrap = false
          }
        }
      }
    }

    Row {
      width: parent.width
      height: 20
      spacing: 4
      Repeater {
        model: details.data.genre
        delegate: Text {
          text: modelData
          wrapMode: Text.WordWrap
        }
      }
    }

    RowLayout {
      width: parent.width - 8
      height: 50
      Layout.margins: 4
      anchors {
        left: parent.left
        leftMargin: 4
      }

      Button {
        Layout.fillWidth: true
        Layout.fillHeight: true
        font.family: "Material Design Icons"
        text: details.data.inLibrary ? qsTr("%1\nIn Library").arg(MdiFont.Icon.heart) : qsTr("%1\nAdd to Library").arg(MdiFont.Icon.heartPlusOutline)
        onClicked:  {
          mangaChanged()
          details.data.inLibrary ? details.removeFromLibrary() : details.addToLibrary()
        }
      }
      Button {
        Layout.fillHeight: true
        Layout.fillWidth: true
        text: qsTr("View in Browser")
        onClicked: details.openUrl()
      }
      Button {
        Layout.fillHeight: true
        Layout.fillWidth: true
        text: qsTr("Download")
        onClicked: popup.open()
      }
    }

    RowLayout {
      width: parent.width
      height: chaptersModel.loading ? 50 : 0
      Text {
        text: qsTr("loading new chapters...")
        visible: chaptersModel.loading
        Layout.alignment: Qt.AlignRight
        font.pixelSize: 20
        Layout.fillWidth: true
        Layout.leftMargin: 4
      }
      BusyIndicator {
        id: busyIndicator
        running: chaptersModel.loading
        Layout.alignment: Qt.AlignLeft
        Layout.rightMargin: 4
      }
    }
  }

  ListView {
    id: chapterView
    clip: true
    anchors {
      bottom: parent.bottom
      left: parent.left
      right: parent.right
    }
    height: {
      return base.height - detailsColumn.childrenRect.height - 4
    }

    model: chaptersModel
    delegate: SwipeDelegate {
      width: chapterView.width
      height: 60

      property bool timerTriggered: false
      Timer {
        id: longPressTimer

        interval: 800 //your press-and-hold interval here
        repeat: false
        running: false

        onTriggered: {
          timerTriggered = true;
          chapterNumberPopup = chapterIndex
          console.log("long press chapter is: ", read)
          chapterReadPopup = read
          popupChapter.open()
        }
      }
      onClicked: {
        if (!timerTriggered) {
          const viewer = navigatePage(Qt.resolvedUrl("Viewer.qml"),
                                       { mangaNumber: detailsModel.mangaNumber,
                                         chapter: chapterIndex })
          viewer.chapterRead.connect(markRead)
        }
        timerTriggered = false;
      }
      onPressedChanged: {
        if ( pressed && !swipe.position ) {
          longPressTimer.running = true;
        } else {
          longPressTimer.running = false;
        }
      }
      swipe.onPositionChanged: longPressTimer.running = false;
      swipe.right: Rectangle {
        id: rightLabel
        color: "#2E7D32"
        width: parent.width
        height: parent.height
        anchors.right: parent.right
        Text {
          id: icon
          anchors {
            right: parent.right
            rightMargin: 12
            top: parent.top
            bottom: parent.bottom
          }

          text: read ? MdiFont.Icon.eyeOff : MdiFont.Icon.eye
          font.family: "Material Design Icons"
          font.pixelSize: 20
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          color: "#F5F5F5"
        }
      }
      swipe.onOpened: {
        chaptersModel.chapterRead(chapterIndex, !read)
        swipe.close()
      }
      contentItem: Rectangle {
        color: "#212121"
        border {
          width: 1
          color: "#F5F5F5"
        }
        Text {
          anchors {
            left: parent.left
            right: downloadedText.left
            top: parent.top
            bottom: parent.bottom
            margins: 4
          }
          text: name
          color: read ? "grey" : "#F5F5F5"
          horizontalAlignment: Text.AlignCenter
          verticalAlignment: Text.AlignVCenter
          leftPadding: 12
          font.pixelSize: 24
          fontSizeMode: Text.Fit
        }
        Item {
          id: downloadedText
          anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
          }

          width: parent.height * .75
          Text {
            id: downloadStatus
            visible: progress < 0 || progress >= 100
            anchors.fill: parent
            font.family: "Material Design Icons"
            color: "#F5F5F5"
            text: downloaded ? MdiFont.Icon.checkCircle : MdiFont.Icon.downloadCircleOutline
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
            rightPadding: 12
            font.pixelSize: 24
          }
          RadialBarShape {
            anchors {
              centerIn: parent
            }
            height: parent.height * .90
            width: parent.width * .90
            visible: progress > 0 && progress < 100
            progressColor: "#e6436d"
            value: progress
            spanAngle: 270
            dialType: RadialBarShape.DialType.FullDial
            backgroundColor: "#6272a4"
            penStyle: Qt.FlatCap
            dialColor: "transparent"
          }
          MouseArea {
            anchors.fill: parent
            onClicked: {
              chaptersModel.downloadChapter(ChaptersModel.DownloadCustom, chapterIndex)
            }
          }
        }
      }
    }

    PullToRefreshHandler {
      id: pulldown_handler
      onPulldownrelease: {
        chaptersModel.requestChapters(true)
      }
    }
  }

  Rectangle {
    visible: chaptersModel.lastReadChapter > 0
    width: 100
    height: 40
    radius: 15
    anchors {
      right: parent.right
      bottom: parent.bottom
      margins: 8
    }
    color: "#0492c2"
    Text {
      text: qsTr("▶ Start")
      color: "#F5F5F5"
      anchors.centerIn: parent
      font.pixelSize: 20
      font.bold: true
      fontSizeMode: Text.Fit
    }
    MouseArea {
      anchors.fill: parent
      onClicked: {
        const viewer = navigatePage(Qt.resolvedUrl("Viewer.qml"),
                                     { mangaNumber: detailsModel.mangaNumber,
                                       chapter: chaptersModel.lastReadChapter })
        viewer.chapterRead.connect((chapter) => {
          chaptersModel.chapterRead(chapter, true)
        })
      }
    }
  }
}
