import QtQuick
import QtQuick.Layouts

Item {
  id: base
  property alias image: icon.text
  property alias text: label.text
  signal clicked()
  ColumnLayout {
    anchors.fill: parent
    Text {
      id: icon
      Layout.fillHeight: true
      Layout.fillWidth: true
      font.family: "Material Design Icons"
      font.pixelSize: 20
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
      color: "#F5F5F5"
    }
    Text {
      id: label
      Layout.fillWidth: true
      font.family: "Roboto"
      font.pixelSize: 12
      horizontalAlignment: Text.AlignHCenter
      color: "#F5F5F5"
    }
  }
  MouseArea {
    anchors.fill: parent
    onClicked: base.clicked()
  }
}
