import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import "../../libs/QmlBridgeForMaterialDesignIcons/Icon.js" as MdiFont

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
    id: authPopup
    width: parent.width * .80
    height: parent.height * .50
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
          text: qsTr("Username")
          font.pixelSize: 18
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          fontSizeMode: Text.Fit
        }
        TextField {
          id: usernameText
          Layout.fillWidth: true
          Layout.fillHeight: true
          font.pixelSize: 18
          color: "#F5F5F5"
          text: qsTr("%1").arg(settings.username)
        }
        Text {
          color: "#F5F5F5"
          Layout.fillWidth: true
          Layout.fillHeight: true
          text: qsTr("Password")
          font.pixelSize: 18
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          fontSizeMode: Text.Fit
        }
        TextField {
          id: passwordText
          Layout.fillWidth: true
          Layout.fillHeight: true
          font.pixelSize: 18
          color: "#F5F5F5"
          text: qsTr("%1").arg(settings.password)
          passwordMaskDelay: 2
          echoMode: TextInput.PasswordEchoOnEdit
        }
        Button {
          Layout.fillWidth: true
          Layout.preferredHeight: 75
          text: qsTr("Enter")
          onClicked: {
             settings.username = usernameText.text
             settings.password = passwordText.text
             authPopup.close()
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

  ViewerPopup {
    id: viewerPopup
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

    SettingsImageInfo {
      Layout.fillWidth: true
      Layout.preferredHeight: 75
      header: "Server's Hostname"
      value: settings.hostname
      image: MdiFont.Icon.web
      onClicked: hostnamePopup.open()
    }

    SettingsImageInfo {
      Layout.fillWidth: true
      Layout.preferredHeight: 75
      header: "Authorization"
      value: settings.username
      image: MdiFont.Icon.security
      onClicked: authPopup.open()
    }

    SettingsImageInfo {
      Layout.fillWidth: true
      Layout.preferredHeight: 75
      header: "Language"
      value: settings.lang
      image: MdiFont.Icon.translate
      onClicked: languagePopup.open()
    }

    SettingsImageInfo {
      Layout.fillWidth: true
      Layout.preferredHeight: 75
      header: qsTr("Default Reader Mode")
      property var viewerModes: [ "WebToon", "Manga" ]
      value: viewerModes[settings.viewer]
      image: MdiFont.Icon.bookOpen
      onClicked: viewerPopup.open()
    }

    SettingsSwitch {
      Layout.fillWidth: true
      Layout.preferredHeight: 75
      switchText: qsTr("NSFW extensions")
      switchSetting: settings.nsfw
      onSwitchClicked: settings.nsfw = !settings.nsfw
    }

    SettingsSwitch {
      Layout.fillWidth: true
      Layout.preferredHeight: 75
      switchText: qsTr("Auto update chapters when opening manga")
      switchSetting: settings.autoUpdate
      onSwitchClicked: settings.autoUpdate = !settings.autoUpdate
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
