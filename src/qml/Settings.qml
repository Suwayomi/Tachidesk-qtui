import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Rectangle {
  color: "#212121"

  Popup {
    id: hostnamePopup
    width: parent.width * .80
    height: parent.height * .25
    focus: true
    modal: true
    anchors.centerIn: parent

    Rectangle {
      anchors.fill: parent
      color: "#212121"
      ColumnLayout {
        anchors.fill: parent
        anchors.margins: 4
        spacing: 4
        Text {
          color: "#F5F5F5"
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
          color: "#F5F5F5"
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

  Popup {
    id: languagePopup
    width: parent.width * .80
    height: parent.height * .25
    focus: true
    modal: true
    anchors.centerIn: parent

    Rectangle {
      anchors.fill: parent
      color: "#212121"
      ColumnLayout {
        anchors.fill: parent
        anchors.margins: 4
        spacing: 4
        Text {
          color: "#F5F5F5"
          Layout.fillWidth: true
          Layout.fillHeight: true
          text: qsTr("Filter Sources by Language")
          font.pixelSize: 18
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          fontSizeMode: Text.Fit
        }
        TextField {
          Layout.fillWidth: true
          Layout.fillHeight: true
          font.pixelSize: 18
          color: "#F5F5F5"
          text: settings.lang
          inputMethodHints: Qt.ImhUrlCharactersOnly
          onAccepted:  {
            settings.lang = text
            languagePopup.close()
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
    spacing: 4

    Text {
      color: "#F5F5F5"
      Layout.fillWidth: true
      Layout.fillHeight: true
      text: qsTr("Settings")
      font.pixelSize: 24
      font.bold: true
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
      fontSizeMode: Text.Fit
    }

    RowLayout {
      Layout.fillWidth: true
      Layout.preferredHeight: 100
      spacing: 4
      Text {
        color: "#F5F5F5"
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
        color: "#F5F5F5"
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
        color: "#F5F5F5"
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
        MouseArea {
          anchors.fill: parent
          onClicked: languagePopup.open()
        }
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
