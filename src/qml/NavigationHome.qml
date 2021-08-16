import QtQuick 2.8
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.15

Item {
  ColumnLayout {
    anchors {
      fill: parent
      margins: 4
    }
    Button {
      Layout.fillWidth: true
      Layout.fillHeight: true
      text: qsTr("Library")
      onClicked: navigatePage(Qt.resolvedUrl("Library.qml"), {})
    }
    Button {
      Layout.fillWidth: true
      Layout.fillHeight: true
      text: qsTr("Sources")
      onClicked: navigatePage(Qt.resolvedUrl("Sources.qml"), {})
    }
    Button {
      Layout.fillWidth: true
      Layout.fillHeight: true
      text: qsTr("Extensions")
      onClicked: navigatePage(Qt.resolvedUrl("Extensions.qml"), {})
    }
    Button {
      Layout.fillWidth: true
      Layout.fillHeight: true
      text: qsTr("Downloads")
    }
    Button {
      Layout.fillWidth: true
      Layout.fillHeight: true
      text: qsTr("Settings")
    }
  }
}
