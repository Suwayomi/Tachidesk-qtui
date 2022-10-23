import QtQuick
import QtQuick.Window

import Tachidesk.Qtui

Item {
  id: base
  property alias model: listView.model
  signal chapterRead(int mangaId, int chapter)
  function imageLoaded() {
    if (lastReadPage && !positioned) {
      listView.currentIndex = lastReadPage
      positioned = true
    }
  }


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
        const chapterIndex = model.get(currentIndex).chapterIndex
        console.log("chapter index, manga number: ", chapterIndex, chapterModel.mangaNumber)
        chapterRead(chapterModel.mangaNumber, chapterIndex)
        chapterModel.requestChapter(chapterIndex + 1)
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

        const indexIs = listView.indexAt(listView.contentX, listView.contentY + base.height - 10)
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
