import QtQuick
import QtQuick.Window

import Tachidesk.Qtui

Item {
  id: base
  property alias model: listView.model
  signal chapterRead(int chapterId, int chapter)
  function imageLoaded() {
    if (lastReadPage && !positioned) {
      listView.currentIndex = lastReadPage
      positioned = true
    }
  }


  ListView {
    id: listView
    anchors.fill: parent
    maximumFlickVelocity: 10000
    //contentWidth:  pinchArea.width
    synchronousDrag: true
    // Only keep ~4 screens worth of delegates alive
    cacheBuffer: height * 4
    // Reuse delegate items aggressively
    // reuseItems: true
    delegate: WebtoonImage {
      imageIndex: index
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
      model.updateChapter(indexIs)
    }
    onAtYBeginningChanged: {
      if (!model.pageCount || model.requestingChapter) {
        return
      }
      console.log("at the start")
      if (contentY <= 0) {
        console.log("at the start, requesting previous chapter")
        model.requestNext(false)
      }
    }

    onAtYEndChanged: {
      if (!model.pageCount || model.requestingChapter) {
        return
      }
      if (listView.atYEnd) {
        const chapterIndex = model.getLastChapter()
        chapterRead(model.chapterId, chapterIndex)
        model.requestNext(true)
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
        model.updateChapter(indexIs)
        if (listView.atYend) {
          const chapterIndex = model.getLastChapter()
          model.requestNext(true)
        }

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
