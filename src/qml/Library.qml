import QtQuick 2.8
import QtQuick.Layouts 1.15

import Tachidesk.Models 1.0

Item {
  id: base
  LibraryModel {
    id: libraryModel
    nm: networkManager
  }
  //property int cellSize: (base.width / 150)
  function setHeight(height) {
    if (height > grid.cellHeight) {
      grid.cellHeight = height + 20
    }
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
      //width: image.width > listView.width ? listView.width - 8 : image.width + 4
      //height: image.height + infoText.height + 4

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
        onClicked: navigatePage(Qt.resolvedUrl("MangaDetails.qml"), { mangaNumber: mangaId })
      }
    }
  }
}
