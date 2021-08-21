import QtQuick 2.8
import QtQuick.Layouts 1.15

import Tachidesk.Models 1.0

Item {
  id: libraryBase
  LibraryModel {
    id: libraryModel
    nm: networkManager
  }

  function setHeight(height) {
    if (height > grid.cellHeight) {
      grid.cellHeight = height + 20
    }
  }

  function onMangaChanged() {
    libraryModel.refreshLibrary()
  }

  GridView {
    id: grid
    anchors.fill: parent
    model: libraryModel
    cellWidth: 200
    clip: true
    delegate: Rectangle {
      width: grid.cellWidth
      height: grid.cellHeight

      Image {
        id: image
        anchors {
          top: parent.top
          left: parent.left
          right: parent.right
          margins: 4
        }
        fillMode: Image.PreserveAspectFit
        source: thumbnailUrl
        onStatusChanged: {
          if (image.status == Image.Ready) {
            setHeight(image.height)
          }
        }
      }

      border {
        width: 2
      }
      Text {
        id: infoText
        anchors {
          top: image.bottom
          topMargin: 4
        }

        text: title
      }
      MouseArea {
        anchors.fill: parent
        onClicked: {
          var details = navigatePage(Qt.resolvedUrl("MangaDetails.qml"), { mangaNumber: mangaId })
          details.mangaChanged.connect(onMangaChanged)
        }
      }
    }
  }
}
