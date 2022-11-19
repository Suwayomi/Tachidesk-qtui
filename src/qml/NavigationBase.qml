import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import "../../libs/QmlBridgeForMaterialDesignIcons/Icon.js" as MdiFont

Item {
  SwipeView {
    id: view

    anchors {
      left: parent.left
      right: parent.right
      top: parent.top
      bottom: indicator.top
    }

    Repeater {
      model: [ "Library.qml", "Updates.qml", "SourceExtensionsBase.qml", "Downloads.qml", "Settings.qml" ]
      Loader {
        id: delegate
        //active: SwipeView.isCurrentItem || SwipeView.isNextItem || SwipeView.isPreviousItem
        property bool isCurrentItem: SwipeView.isCurrentItem
        source: modelData
        onVisibleChanged: {
          if (isCurrentItem && visible) {
            delegate.item.refresh()
          }
        }
        Connections {
          target: view
          function onCurrentItemChanged() {
            if (isCurrentItem) {
              delegate.item.refresh()
            }
          }
        }
      }
    }
  }
  ListModel {
    id: navigationModel
    ListElement { name: "Library"; icon: "\u{f0B64}" } //MdiFont.Icon.bookOutline }
    ListElement { name: "Updates"; icon: "\u{f0CE5}" } //MdiFont.Icon.alertDecagramOutline}
    ListElement { name: "Browse"; icon: "\u{f018C}" } //MdiFont.Icon.compassOutline }
    ListElement { name: "Downloads"; icon: "\u{f0B8F}" } //MdiFont.Icon.downloadOutline }
    ListElement { name: "Settings"; icon: "\u{f01D8}" } //MdiFont.Icon.dotsHorizontal }
  }
  RowLayout {
    id: indicator
    anchors.bottom: parent.bottom
    //anchors.horizontalCenter: parent.horizontalCenter
    height: 50
    width: parent.width
    spacing: 8
    Repeater {
      model: navigationModel
      delegate: Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
        //width: pageText.contentWidth + 10
        //height: pageText.contentHeight + 10
        Rectangle {
          visible: index === view.currentIndex
          anchors.fill: parent
          radius: 10
          color: "#0492c2"
        }
        IconButton {
          id: pageText
          text: name
          image: icon
          anchors.fill: parent
        }
        MouseArea {
          anchors.fill: parent
          onClicked: view.currentIndex = index
        }
      }
    }
  }
}
