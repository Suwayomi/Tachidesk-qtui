import QtQuick 2.8
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.15

Item {
  RowLayout {
    anchors {
      fill: parent
      margins: 4
    }
    Button {
      Layout.fillWidth: true
      Layout.fillHeight: true
      text: qsTr("Library")
      onClicked: navigatePage(Qt.resolvedUrl("Library.qml"), {replace: true})
    }
    Button {
      Layout.fillWidth: true
      Layout.fillHeight: true
      text: qsTr("Updates")
      onClicked: navigatePage(Qt.resolvedUrl("Updates.qml"), {replace: true})
    }
    Button {
      Layout.fillWidth: true
      Layout.fillHeight: true
      text: qsTr("Sources")
      onClicked: navigatePage(Qt.resolvedUrl("Sources.qml"), {replace: true})
    }
    Button {
      Layout.fillWidth: true
      Layout.fillHeight: true
      text: qsTr("Extensions")
      onClicked: navigatePage(Qt.resolvedUrl("Extensions.qml"), {replace: true})
    }
    Button {
      Layout.fillWidth: true
      Layout.fillHeight: true
      text: qsTr("Downloads")
      onClicked: navigatePage(Qt.resolvedUrl("Downloads.qml"), {replace: true})
    }
    Button {
      Layout.fillWidth: true
      Layout.fillHeight: true
      text: qsTr("Settings")
      onClicked: navigatePage(Qt.resolvedUrl("Settings.qml"), {replace: true})
    }
  }
}
