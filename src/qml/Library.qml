import QtQuick
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import QtQuick.Controls

import Tachidesk.Models
import SortFilterProxyModel

Item {
  id: libraryBase
  SortFilterProxyModel {
    id: libraryModel
    sourceModel: library
    filterRoleName: "title"
    filterCaseSensitivity: Qt.CaseInsensitive
    sorters: [
      RoleSorter {
        roleName: "title"
        sortOrder: Qt.AscendingOrder
      }
    ]
  }

  LibraryModel {
    id: library
    nm: networkManager
  }

  Rectangle {
    anchors.fill: parent
    color: "#444444"
  }

  Text {
    id: title
    anchors {
      top: parent.top
      left: parent.left
      margins: 4
    }
    color: "white"
    text: qsTr("Library")
    font.pixelSize: 24
    font.bold: true
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    fontSizeMode: Text.Fit
  }

  RowLayout {
    id: searchField
    height: 40
    width: parent.width / 2
    anchors {
      top: parent.top
      left: title.right
      leftMargin: 8
      right: parent.right
    }
    spacing: 0
    TextField {
      id: searchBox
      leftPadding: 0
      color: "white"
      font.pointSize: 12
      horizontalAlignment: TextInput.AlignRight
      Layout.fillWidth: true
      Layout.fillHeight: true
      background: Rectangle {
          color: "transparent"
      }
      placeholderText: qsTr("Search")
      onTextEdited: libraryModel.filterPattern = text
    }
    Image {
      id: searchIcon
      Layout.alignment: Qt.AlignCenter
      Layout.maximumWidth: 50
      Layout.fillWidth: true
      Layout.fillHeight: true
      scale: 0.5
      fillMode: Image.PreserveAspectFit
      source: "qrc:/Tachidesk/qtui/images/outline_search_white_24dp.png"
    }
  }


  LibraryBase {
    anchors {
      top: title.bottom
      left: parent.left
      right: parent.right
      bottom: parent.bottom
    }
  }
}
