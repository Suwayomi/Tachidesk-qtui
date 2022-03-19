import QtQuick

Item {
  id: base
  property alias image: icon.text
  property alias text: label.text
  signal clicked()
  Text {
    id: icon
    anchors {
      left: parent.left
      right: parent.right
      top: parent.top
    }
    font.family: "Material Design Icons"
    font.pixelSize: 20
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    color: "#F5F5F5"
  }
  Text {
    id: label
    anchors {
      left: parent.left
      right: parent.right
      bottom: parent.bottom
    }
    font.family: "Roboto"
    font.pixelSize: 12
    horizontalAlignment: Text.AlignHCenter
    color: "#F5F5F5"
  }
  MouseArea {
    anchors.fill: parent
    onClicked: base.clicked()
  }
}
