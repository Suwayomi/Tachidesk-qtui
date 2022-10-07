import QtQuick
import QtQuick.Controls
import QtQuick.Window

import Tachidesk.Models

Item {
  id: base
  property alias model: listView.model
  signal chapterRead(int mangaId, int chapter)

  ListView {
    id: listView
    anchors.fill: parent
    cacheBuffer: 10000
    maximumFlickVelocity: 10000
    //contentWidth:  pinchArea.width
    synchronousDrag: true
    delegate: WebtoonImage {
      onImageLoaded: base.imageLoaded()
    }
    // debug rectangle
    //delegate: Rectangle {
    //  width: base.width
    //  height: 500
    //  Text {
    //    font.pixelSize: 30
    //    width: base.width
    //    wrapMode: Text.Wrap
    //    text: name +  chapterUrl
    //  }
    //}


    property bool listviewLoaded: false
    Component.onCompleted: {
      listviewLoaded = true
    }
    onMovementStarted: {
      headerBar.height = 0
    }

    onMovementEnded: {
      const indexIs = indexAt(contentX,contentY + base.height - 10)
      chapterModel.updateChapter(indexIs)
    }
    onAtYEndChanged: {
      if (!listviewLoaded) {
        return
      }
      if (listView.atYEnd) {
        chapterRead(chapterModel.mangaNumber, chapterModel.chapterNumber)
        chapterModel.chapterNumber++
        chapterModel.requestChapter(chapterModel.chapterNumber)
      }
    }

    MouseArea {
      anchors.centerIn: parent
      height: parent.height / 2
      width: parent.width / 2
      onClicked: {
        !headerBar.height ? headerBar.height = 55 : headerBar.height = 0
      }
    }

    MouseArea {
      anchors {
        right: parent.right
        top: parent.top
        bottom: parent.bottom
      }
      width: base.width / 4
      onClicked: (mouse) => {
        if (headerBar.height) {
          headerBar.height = 0
        }

        const moveHeight = listView.height - 50
        listView.contentY += moveHeight

        const indexIs = indexAt(contentX, contentY + base.height - 10)
        chapterModel.updateChapter(indexIs)

        mouse.accepted = false
      }
      propagateComposedEvents: true
    }
    MouseArea {
      anchors {
        left: parent.left
        top: parent.top
        bottom: parent.bottom
      }
      width: base.width / 4
      onClicked: (mouse) => {
        if (headerBar.height) {
          headerBar.height = 0
        }
        const moveHeight = listView.height - 50
        if (listView.contentY - moveHeight > 0) {
          listView.contentY -= moveHeight
        }
        else {
          listView.contentY = 0
        }
        mouse.accepted = false
      }
      propagateComposedEvents: true
    }
  }

}
