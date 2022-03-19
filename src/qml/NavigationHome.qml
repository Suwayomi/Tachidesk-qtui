import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import "../../libs/QmlBridgeForMaterialDesignIcons/Icon.js" as MdiFont

Rectangle {
  color: "#424242"
  RowLayout {
    spacing: 0
    anchors {
      fill: parent
      margins: 4
    }
    IconButton {
      Layout.fillWidth: true
      Layout.fillHeight: true
      text: qsTr("Library")
      image: MdiFont.Icon.bookOutline
      onClicked: navigatePage(Qt.resolvedUrl("Library.qml"), {replace: true})
    }
    IconButton {
      Layout.fillWidth: true
      Layout.fillHeight: true
      text: qsTr("Updates")
      image: MdiFont.Icon.alertDecagramOutline
      onClicked: navigatePage(Qt.resolvedUrl("Updates.qml"), {replace: true})
    }
    IconButton {
      Layout.fillWidth: true
      Layout.fillHeight: true
      text: qsTr("Sources")
      image: MdiFont.Icon.compassOutline
      onClicked: navigatePage(Qt.resolvedUrl("Sources.qml"), {replace: true})
    }
    IconButton {
      Layout.fillWidth: true
      Layout.fillHeight: true
      text: qsTr("Extensions")
      image: MdiFont.Icon.storeOutline
      onClicked: navigatePage(Qt.resolvedUrl("Extensions.qml"), {replace: true})
    }
    IconButton {
      Layout.fillWidth: true
      Layout.fillHeight: true
      text: qsTr("Downloads")
      image: MdiFont.Icon.downloadOutline
      onClicked: navigatePage(Qt.resolvedUrl("Downloads.qml"), {replace: true})
    }
    IconButton {
      Layout.fillWidth: true
      Layout.fillHeight: true
      text: qsTr("Settings")
      image: MdiFont.Icon.dotsHorizontal
      onClicked: navigatePage(Qt.resolvedUrl("Settings.qml"), {replace: true})
    }
  }
}
