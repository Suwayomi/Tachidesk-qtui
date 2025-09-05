import QtQuick

import Tachidesk.Qtui

Rectangle {
  id: libraryBase

  property int minimumWidth: 75
  property int maximumWidth: 175
  property bool isLoading: true
  signal reachedEnd()
  function onMangaChanged() {
    libraryModel.refreshLibrary()
  }
  
  Connections {
    target: libraryModel
    function onModelReset() {
      isLoading = libraryModel.rowCount() === 0
    }
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

  color: "#212121"

  GridView {
    id: grid
    anchors.fill: parent
    model: isLoading ? 12 : libraryModel
    cellWidth: getGridWidth()
    cellHeight: getGridHeight()
    clip: true

    property bool listviewLoaded: false
    Component.onCompleted: {
      listviewLoaded = true
    }
    onAtYEndChanged: {
      if (!listviewLoaded || !flicking || isLoading) {
        return
      }
      reachedEnd()
    }
    delegate: isLoading ? placeholderDelegate : mangaDelegate
  }
  
  Component {
    id: placeholderDelegate
    LibraryCardPlaceholder {
      width: grid.cellWidth
      height: grid.cellHeight
    }
  }
  
  Component {
    id: mangaDelegate
    Item {
      width: grid.cellWidth
      height: grid.cellHeight

      Image {
        id: image
        anchors {
          fill: parent
          margins: 4
        }
        fillMode: Image.PreserveAspectCrop
        source: "image://network/" + thumbnailUrl

        visible: false
      }


      ShaderEffect {
        anchors.fill: parent

        // must match the single sampler name in the GLSL:
        property var source: image

        fragmentShader: "qrc:/shaders/darken_mask.frag.qsb"
      }

      Text {
        id: infoText
        anchors {
          margins: 5
          bottom: image.bottom
          left: image.left
          right: image.right
        }
        color: "#F5F5F5"
        font.pixelSize: 14
        //font.weight: Font.DemiBold
        wrapMode: Text.WordWrap
        maximumLineCount: 3
        text: title
        style: Text.Outline
        styleColor: "black"
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
          color: "#F5F5F5"
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