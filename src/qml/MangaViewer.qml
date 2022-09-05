import QtQuick
import QtQuick.Controls
import QtQuick.Window

import Tachidesk.Models

Item {
  id: base
  anchors.fill: parent
  property alias model: imageRepeater.model

  property bool listviewLoaded: false
  SwipeView {
    id: listView
    anchors.fill: parent
    Repeater {
      id: imageRepeater
      Loader {
        active: SwipeView.isCurrentItem || SwipeView.isNextItem || SwipeView.isPreviousItem
        Component.onCompleted: {
          listviewLoaded = true
        }

        sourceComponent: Item {
          anchors.fill: parent
          WebtoonImage {
            anchors.centerIn: parent
            //onImageLoaded: base.imageLoaded()
          }
          MouseArea {
            anchors.centerIn: parent
            height: base.height / 2
            width: base.width / 2
            onClicked: (mouse) => {
              !headerBar.height ? headerBar.height = 55 : headerBar.height = 0
              mouse.accepted = false
            }
            propagateComposedEvents: true
          }
        }
      }
    }

    Connections {
      target: listView
      function onCurrentIndexChanged() {
        if (!listviewLoaded) {
          return
        }

        if (headerBar.height) {
          headerBar.height = 0
        }

        chapterModel.updateChapter(listView.currentIndex)
        if ((listView.currentIndex + 1) == listView.count) {
          chapterRead(chapterModel.mangaNumber, chapterModel.chapterNumber)
          chapterModel.chapterNumber++
          chapterModel.requestChapter(chapterModel.chapterNumber)
        }
      }
    }

  }
}
