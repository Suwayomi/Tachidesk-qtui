import QtQuick 2.8
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.12
import QtQuick.Window 2.2
import QtWebSockets 1.0

import Tachidesk.Models 1.0

Item {
  id: base
  property alias mangaNumber: detailsModel.mangaNumber
  property var doWrap: false
  property int chapterNumberPopup

  signal mangaChanged();

  MangaDetailsModel {
    id: detailsModel
    nm: networkManager
    onLoaded: details = detailsModel.get(0)
  }

  ChaptersModel {
    id: chaptersModel
    nm: networkManager
    mangaNumber: detailsModel.mangaNumber
  }

  function markRead(chapter) {
    chaptersModel.chapterRead(chapter)
  }

  property var details: detailsModel.get(0)

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
      Button {
        Layout.fillWidth: true
        Layout.fillHeight: true
        text: qsTr("Download Custom")
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
          chaptersModel.chapterRead(chapterNumberPopup)
          popupChapter.close()
        }
      }
    }
  }

  Image {
    id: backgroundImage
    source: details.thumbnailUrl
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
        source: details.thumbnailUrl
        fillMode: Image.PreserveAspectFit
        sourceSize.height: detailsColumn.height * 1/1.5
      }
      ColumnLayout {
        Layout.fillWidth: true
        Text {
          Layout.fillWidth: true
          font.pixelSize: 20
          fontSizeMode: Text.Fit
          text: details.title
        }
        Text {
          Layout.fillWidth: true
          font.pixelSize: 20
          fontSizeMode: Text.Fit
          text: details.author
        }
        Text {
          Layout.fillWidth: true
          font.pixelSize: 20
          fontSizeMode: Text.Fit
          text: details.artist
        }
        Text {
          Layout.fillWidth: true
          fontSizeMode: Text.Fit
          font.pixelSize: 10
          text: "%1 - %2".arg(details.status).arg(details.sourceName)
          color: "#222222"
        }
      }
    }

    Text {
      text: details.description
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
    Text {
      text: details.genre
      wrapMode: Text.WordWrap
      width: parent.width
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
        text: details.inLibrary ? qsTr("❤\nIn Library") : qsTr("♡\nAdd to Library")
        onClicked:  {
          mangaChanged()
          details.inLibrary ? detailsModel.removeFromLibrary() : detailsModel.addToLibrary()
          details = detailsModel.get(0)
        }
      }
      Button {
        Layout.fillHeight: true
        Layout.fillWidth: true
        text: qsTr("View in Browser")
      }
      Button {
        Layout.fillHeight: true
        Layout.fillWidth: true
        text: qsTr("Download")
        onClicked: popup.open()
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
    delegate: Rectangle {
      width: chapterView.width
      height: 60
      color: "#333333"
      border {
        width: 1
        color: "white"
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
        color: read ? "grey" : "white"
        horizontalAlignment: Text.AlignCenter
        verticalAlignment: Text.AlignVCenter
        leftPadding: 12
        font.pixelSize: 24
        fontSizeMode: Text.Fit
      }
      Text {
        id: downloadedText
        anchors {
          right: parent.right
          top: parent.top
          bottom: parent.bottom
        }
        color: "white"
        text: downloaded ? "✅" : "⬇"
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
        rightPadding: 12
        font.pixelSize: 24
      }
      MouseArea {
        anchors.fill: parent
        onClicked: {
          var viewer = navigatePage(Qt.resolvedUrl("WebtoonViewer.qml"),
                                       { mangaNumber: detailsModel.mangaNumber,
                                         chapter: chapterIndex })
          viewer.chapterRead.connect(markRead)
        }
        onPressAndHold: {
          chapterNumberPopup = chapterIndex
          popupChapter.open()
        }
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
      color: "white"
      anchors.centerIn: parent
      font.pixelSize: 20
      font.bold: true
      fontSizeMode: Text.Fit
    }
    MouseArea {
      anchors.fill: parent
      onClicked: {
        console.log(chaptersModel.lastReadChapter)
          var viewer = navigatePage(Qt.resolvedUrl("WebtoonViewer.qml"),
                                       { mangaNumber: detailsModel.mangaNumber,
                                         chapter: chaptersModel.lastReadChapter })
          viewer.chapterRead.connect((chapter) => {
            chaptersModel.chapterRead(chapter)
          })
        }
    }
  }
}
