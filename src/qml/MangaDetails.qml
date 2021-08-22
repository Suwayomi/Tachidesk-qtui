import QtQuick 2.8
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Tachidesk.Models 1.0

Item {
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

    RowLayout {
      width: parent.width

      Image {
        id: image
        source: details.thumbnailUrl
        fillMode: Image.PreserveAspectFit
        sourceSize.height: 250
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
          text: details.status //+ " " + source.name
        }
      }
    }

    Text {
      text: details.description
      //wrapMode: Text.WordWrap
      elide: Text.ElideRight
      width: parent.width
      MouseArea {
        anchors.fill: parent
        onClicked: {
          if (!doWrap) {
            parent.wrapMode = Text.WordWrap
            doWrap = true
          } else {
            parent.wrapMode = Text.NoWrap
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
      height: 150
      Layout.margins: 4
      anchors.left: parent.left
      anchors.leftMargin: 4

      Button {
        Layout.fillWidth: true
        text: details.inLibrary ? qsTr("In Library") : qsTr("Add to Library")
        onClicked:  {
          mangaChanged()
          details.inLibrary ? detailsModel.removeFromLibrary() : detailsModel.addToLibrary()
          details = detailsModel.get(0)
        }
      }
      Button {
        Layout.fillWidth: true
        text: qsTr("View in Browser")
        onClicked: detailsModel.test();
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
      top: detailsColumn.bottom
    }

    model: chaptersModel
    delegate: Button {
      width: chapterView.width
      height: 150
      text: qsTr("chapter: %1").arg(name)
      onClicked: {
        navigatePage(Qt.resolvedUrl("WebtoonViewer.qml"),
                                     { mangaNumber: detailsModel.mangaNumber,
                                         chapter: chapterIndex })
      }
    }
  }
}
