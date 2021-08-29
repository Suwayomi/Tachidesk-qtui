import QtQuick 2.8
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Tachidesk.Models 1.0

Item {
  id: base
  property alias mangaNumber: detailsModel.mangaNumber
  property var doWrap: false

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

  property var details: detailsModel.get(0)


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
        sourceSize.height: detailsColumn.height * 1/3
      }
      ColumnLayout {
        Layout.fillWidth: true
        Text {
          text: details.title
        }
        Text {
          text: details.author
        }
        Text {
          text: details.artist
        }
        Text {
          text: details.status
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
        text: details.inLibrary ? qsTr("In Library") : qsTr("Add to Library")
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
      height: 100
      color: "black"
      border {
        width: 1
        color: "white"
      }
      Text {
        text: name
        color: read ? "grey" : "white"
        anchors.fill: parent
        horizontalAlignment: Text.AlignCenter
        verticalAlignment: Text.AlignVCenter
        leftPadding: 12
        font.pixelSize: 24
      }
      MouseArea {
        anchors.fill: parent
        onClicked: {
          var viewer = navigatePage(Qt.resolvedUrl("WebtoonViewer.qml"),
                                       { mangaNumber: detailsModel.mangaNumber,
                                           chapter: chapterIndex })
          viewer.chapterRead.connect((chapter) => {
            chaptersModel.chapterRead(chapter)
          })
        }
      }
    }
  }
}
