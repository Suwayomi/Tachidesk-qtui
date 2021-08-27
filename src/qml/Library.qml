import QtQuick 2.8
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.0

import Tachidesk.Models 1.0

Rectangle {
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

  color: "#333333"

  GridView {
    id: grid
    anchors.fill: parent
    model: libraryModel
    cellWidth: 200
    clip: true
    delegate: Item {
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

        OpacityMask {
          source: mask
          maskSource: image
        }

        LinearGradient {
          id: mask
          anchors.fill: image
          gradient: Gradient {
              GradientStop { position: 0.6;  color: "transparent"}
              GradientStop { position: 0.95; color: "black" }
          }
        }

        Text {
          id: infoText
          anchors {
            margins: 4
            bottom: image.bottom
            left: image.left
            right: image.right
          }
          color: "white"
          font.bold: true
          font.pixelSize: 16
          wrapMode: Text.WordWrap
          text: title
          style: Text.Outline
          styleColor: "black"
        }

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
