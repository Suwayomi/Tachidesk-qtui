import QtQuick 2.8
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0

import Tachidesk.Models 1.0
import SortFilterProxyModel 0.2

Rectangle {

  color: "#333333"

  DownloadsModel {
    id: downloadsModel
    nm: networkManager
  }

  Button {
    id: clear
    height: 50
    text: qsTr("clear")
    onClicked: downloadsModel.clear()
    width: parent.width
  }

  ListView {
    id: downloadView
    clip: true
    anchors {
      top: clear.bottom
      right: parent.right
      left: parent.left
      bottom: parent.bottom
    }
    spacing: 4
    model: downloadsModel
    delegate: Rectangle {
      height: 50
      width: downloadView.width

      RowLayout {
        anchors {
          fill: parent
          margins: 4
        }

        Text {
          Layout.fillWidth: true
          text: name
          fontSizeMode: Text.Fit
        }
        Text {
          Layout.fillWidth: true
          text: qsTr("tries %1").arg(tries)
        }
        Text {
          Layout.fillWidth: true
          text: qsTr("progress %1%").arg(progress)
        }
        Button {
          Layout.fillWidth: true
          text: qsTr("cancel")
          onClicked: downloadsModel.cancel(index)
        }
      }
    }
  }
}
