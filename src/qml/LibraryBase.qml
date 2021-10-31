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
        fillMode: Image.PreserveAspectCrop
        source: thumbnailUrl

        OpacityMask {
          source: mask
          maskSource: image
        }

        LinearGradient {
          id: mask
          anchors.fill: image
          gradient: Gradient {
              GradientStop { position: 0.5;  color: "transparent"}
              GradientStop { position: 0.99; color: "black" }
          }
        }

        Text {
          id: infoText
          anchors {
            margins: 5
            bottom: image.bottom
            left: image.left
            right: image.right
          }
          color: "white"
          font.pixelSize: 14
          font.weight: Font.DemiBold
          wrapMode: Text.WordWrap
          maximumLineCount: 3
          text: title
          style: Text.Outline
          styleColor: "black"
        }

      }
      Rectangle {
        visible: unread > 0
        width: unreadText.width + 10
        height: parent.height / 7
        radius: 15
        anchors {
          right: parent.right
          top: parent.top
          margins: 8
        }
        color: "#0492c2"
        Text {
          id: unreadText
          text: unread
          color: "white"
          anchors.centerIn: parent
          font.pixelSize: 14
          font.bold: true
          fontSizeMode: Text.Fit
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
