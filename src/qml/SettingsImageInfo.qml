import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
  id: base
  property alias image: icon.text
  property alias header: headerText.text
  property alias value: valueText.text
  signal clicked()

  RowLayout {
    anchors.fill: parent
    spacing: 8
    Text {
      id: icon
      font.family: "Material Design Icons"
      font.pixelSize: 20
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
      color: "#F5F5F5"
    }

    ColumnLayout {
      Layout.alignment: Qt.AlignLeft
      Text {
        id: headerText
        Layout.fillWidth: true
        color: "#F5F5F5"
        font.pixelSize: 18
        fontSizeMode: Text.Fit
        //horizontalAlignment: Text.AlignHCenter
        //verticalAlignment: Text.AlignVCenter
      }
      Text {
        id: valueText
        Layout.fillWidth: true
        color: "#c2c2c2"
        font.pixelSize: 15
        fontSizeMode: Text.Fit
        //horizontalAlignment: Text.AlignHCenter
        //verticalAlignment: Text.AlignVCenter
      }
    }
  }

  MouseArea {
    anchors.fill: parent
    onClicked: base.clicked()
  }
}
