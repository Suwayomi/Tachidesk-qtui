import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Rectangle {
  color: "#333333"

  Popup {
    id: hostnamePopup
    width: parent.width * .80
    height: parent.height * .25
    focus: true
    modal: true
    anchors.centerIn: parent

    Rectangle {
      anchors.fill: parent
      color: "#333333"
      ColumnLayout {
        anchors.fill: parent
        anchors.margins: 4
        spacing: 4
        Text {
          color: "white"
          Layout.fillWidth: true
          Layout.fillHeight: true
          text: qsTr("Server's Hostname")
          font.pixelSize: 18
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          fontSizeMode: Text.Fit
        }
        TextField {
          Layout.fillWidth: true
          Layout.fillHeight: true
          font.pixelSize: 18
          color: "white"
          text: qsTr("%1").arg(settings.hostname)
          inputMethodHints: Qt.ImhUrlCharactersOnly
          onAccepted:  {
            settings.hostname = text
            hostnamePopup.close()
          }
        }
      }
    }
  }

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
        color: "white"
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
        color: "white"
        readOnly: true
        placeholderText: qsTr("%1").arg(settings.hostname)
        MouseArea {
          anchors.fill: parent
          onClicked: hostnamePopup.open()
        }
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
        color: "white"
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

    Button {
      Layout.fillWidth: true
      Layout.preferredHeight: 100
      text: settings.nsfw ? "NSFW enabled" : "NSFW disabled"
      onClicked: settings.nsfw = !settings.nsfw
    }

    //FileDialog {
    //  id: fileDialog
    //  title: "Backup File"
    //  folder: shortucts.home
    //  onAccepted: {
    //    console.log("you chose:" + fileDialog.fileUrls)
    //  }
    //  Component.onCompleted: visible = true
    //}
  }
}
