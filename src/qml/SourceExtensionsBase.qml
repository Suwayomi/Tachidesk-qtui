import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {

  function refresh() {
    console.log("refresh extension base")
  }

  SwipeView {
    id: sourceView

    anchors {
      left: parent.left
      right: parent.right
      bottom: parent.bottom
      top: sourceIndicator.bottom
    }

    Repeater {
      model: [ "Sources.qml", "Extensions.qml" ]
      Loader {
        id: delegate
        property bool isCurrentItem: SwipeView.isCurrentItem
        source: modelData
        onVisibleChanged: {
          if (isCurrentItem && visible) {
            delegate.item.refresh()
          }
        }
        Connections {
          target: sourceView
          function onCurrentItemChanged() {
            if (isCurrentItem) {
              delegate.item.refresh()
            }
          }
        }
      }
    }
  }

  Row {
    id: sourceIndicator
    anchors.top: parent.top
    anchors.horizontalCenter: parent.horizontalCenter
    spacing: 8
    Repeater {
      model: [ "Sources", "Extensions" ]
      delegate: Item {
        width: pageText.contentWidth + 10
        height: pageText.contentHeight + 10
        Rectangle {
          visible: index === sourceView.currentIndex
          anchors.fill: parent
          radius: 10
          color: "#0492c2"
        }
        Text {
          id: pageText
          text: modelData
          font.pixelSize: 20
          anchors.centerIn: parent
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          color: "#F5F5F5"
        }
        MouseArea {
          anchors.fill: parent
          onClicked: sourceView.currentIndex = index
        }
      }
    }
  }
}
