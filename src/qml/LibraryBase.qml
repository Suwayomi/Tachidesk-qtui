import QtQuick 2.8
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.0

import Tachidesk.Models 1.0

Rectangle {
  id: libraryBase

  property int minimumWidth: 75
  property int maximumWidth: 175
  signal reachedEnd()
  function onMangaChanged() {
    libraryModel.refreshLibrary()
  }

  function getGridWidth() {
    var gridElements = 2
    var gridWidth = grid.width
    while (gridWidth > maximumWidth) {
      gridWidth = grid.width  * 1 / gridElements
      gridElements++
    }
    return gridWidth > minimumWidth ? gridWidth : minimumWidth
  }
  function getGridHeight() {
    return grid.cellWidth * 1.333
  }

  color: "#333333"

  GridView {
    id: grid
    anchors.fill: parent
    model: libraryModel
    cellWidth: getGridWidth()
    cellHeight: getGridHeight()
    clip: true

    property bool listviewLoaded: false
    Component.onCompleted: {
      listviewLoaded = true
    }
    onAtYEndChanged: {
      if (!listviewLoaded) {
        return
      }
      reachedEnd()
    }
    delegate: Item {
      width: grid.cellWidth
      height: grid.cellHeight

      Image {
        id: image
        anchors {
          fill: parent
          margins: 4
        }
        fillMode: Image.PreserveAspectFit
        source: thumbnailUrl

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
