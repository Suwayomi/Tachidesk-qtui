import QtQuick 2.8
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0

Item {
  ColumnLayout {
    anchors {
      top: parent.top
      left: parent.left
      right: parent.right
      margins: 4
    }
    spacing: 2

    RowLayout {
      Layout.fillWidth: true
      Layout.preferredHeight: 100
      spacing: 4
      Text {
        Layout.fillWidth: true
        Layout.preferredHeight: 100
        Layout.preferredWidth: parent.width * .20
        text: qsTr("Server's\nHostname")
        font.pixelSize: 18
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        fontSizeMode: Text.Fit
      }
      TextField {
        id: hostTextField
        Layout.fillWidth: true
        Layout.preferredWidth: parent.width * .75
        Layout.preferredHeight: 100
        font.pixelSize: 18
        placeholderText: qsTr("%1").arg(settings.hostname)
        inputMethodHints: Qt.ImhUrlCharactersOnly
        onAccepted: settings.hostname = text
      }
      Button {
        Layout.fillWidth: true
        Layout.preferredHeight: 100
        text: "enter"
        onClicked: settings.hostname = hostTextField.text
      }
    }

    RowLayout {
      Layout.fillWidth: true
      Layout.preferredHeight: 100
      spacing: 4
      Text {
        Layout.fillWidth: true
        Layout.preferredHeight: 100
        Layout.preferredWidth: parent.width * .20
        text: qsTr("Server's\nPort")
        font.pixelSize: 18
        fontSizeMode: Text.Fit
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

      }

      TextField {
        id: portTextField
        Layout.fillWidth: true
        Layout.preferredHeight: 100
        Layout.preferredWidth: parent.width * .75
        font.pixelSize: 18
        placeholderText: settings.port
        onAccepted: settings.port = text
      }
      Button {
        Layout.fillWidth: true
        Layout.preferredHeight: 100
        text: "enter"
        onClicked: settings.port = portTextField.text
      }
    }

    RowLayout {
      Layout.fillWidth: true
      Layout.preferredHeight: 100
      spacing: 4
      Text {
        Layout.fillWidth: true
        Layout.preferredHeight: 100
        Layout.preferredWidth: parent.width * .20
        text: qsTr("language")
        font.pixelSize: 18
        fontSizeMode: Text.Fit
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
      }

      TextField {
        id: langTextField
        Layout.fillWidth: true
        Layout.preferredHeight: 100
        Layout.preferredWidth: parent.width * .75
        font.pixelSize: 18
        placeholderText: settings.lang
        onAccepted: settings.lang = text
      }
      Button {
        Layout.fillWidth: true
        Layout.preferredHeight: 100
        text: "enter"
        onClicked: settings.lang = langTextField.text
      }
    }
  }
}
