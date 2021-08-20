import QtQuick 2.8
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Tachidesk.Models 1.0

Item {
  property alias mangaNumber: detailsModel.mangaNumber

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
      wrapMode: Text.WordWrap
      width: parent.width
    }
    Text {
      text: details.genre
      wrapMode: Text.WordWrap
      width: parent.width
    }
    RowLayout {
      width: parent.width
      height: 150
      Button {
        text: details.inLibrary ? qsTr("In Library") : qsTr("Add to Library")
        onClicked:  {
          details.inLibrary ? detailsModel.removeFromLibrary() : detailsModel.addToLibrary()
          details = detailsModel.get(0)
        }
      }
      Button {
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
      top: detailsColumn.bottom
    }

    model: chaptersModel
    delegate: Button {
      width: chapterView.width
      height: 150
      text: qsTr("chapter: %1").arg(name)
      onClicked: {
        navigatePage(Qt.resolvedUrl("WebtoonViewer.qml"),
                                     { pagecount: pageCount,
                                         chapter: chapterUrl })
      }
    }
  }
}
