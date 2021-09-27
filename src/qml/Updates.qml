import QtQuick 2.8
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0

import Tachidesk.Models 1.0
import SortFilterProxyModel 0.2

Rectangle {

  color: "#333333"

  UpdatesModel {
    id: updatesModel
    nm: networkManager
  }

  ListView {
    id: downloadView
    clip: true
    anchors.fill: parent
    spacing: 4
    model: updatesModel
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
      }
    }
  }
}
