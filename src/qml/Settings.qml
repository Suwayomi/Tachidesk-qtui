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

    TextField {
      Layout.fillWidth: true
      Layout.preferredHeight: 100
      placeholderText: qsTr("%1").arg(settings.hostname)
      inputMethodHints: Qt.ImhUrlCharactersOnly
      onAccepted: settings.hostname = text
    }

    TextField {
      Layout.fillWidth: true
      Layout.preferredHeight: 100
      placeholderText: qsTr("Port: %1").arg(settings.port)
      onAccepted: settings.port = text
    }
  }
}
