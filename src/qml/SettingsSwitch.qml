import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
  id: base
  property string switchText
  property var switchSetting
  signal switchClicked()
  RowLayout {
    anchors.fill: parent
    spacing: 4
    Text {
      color: "#F5F5F5"
      Layout.fillWidth: true
      Layout.preferredHeight: 75
      Layout.preferredWidth: parent.width * .20
      text: switchText
      font.pixelSize: 18
      verticalAlignment: Text.AlignVCenter
      fontSizeMode: Text.Fit
    }
    Switch {
      Layout.preferredHeight: 75
      Layout.alignment: Qt.AlignRight
      checked: switchSetting
      onClicked: base.switchClicked()
    }
  }
}
