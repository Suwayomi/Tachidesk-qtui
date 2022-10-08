import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
  SwipeView {
    id: view

    anchors {
      left: parent.left
      right: parent.right
      bottom: parent.bottom
      top: indicator.bottom
    }

    Sources { }
    Extensions { }
  }
  Row {
    id: indicator
    anchors.top: parent.top
    anchors.horizontalCenter: parent.horizontalCenter
    spacing: 8
    Repeater {
      model: [ "Sources", "Extensions" ]
      delegate: Item {
        width: pageText.contentWidth + 10
        height: pageText.contentHeight + 10
        Rectangle {
          visible: index == view.currentIndex
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
      }
    }
  }
}
