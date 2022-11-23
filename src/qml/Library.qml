import QtQuick
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import QtQuick.Controls

import Tachidesk.Qtui
import SortFilterProxyModel

import "../../libs/QmlBridgeForMaterialDesignIcons/Icon.js" as MdiFont

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
  }

  function refresh() {
    library.refreshLibrary()
  }

  Rectangle {
    anchors.fill: parent
    color: "#424242"
  }

  Text {
    id: title
    anchors {
      top: parent.top
      left: parent.left
      margins: 4
    }
    color: "#F5F5F5"
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
      color: "#F5F5F5"
      font.pointSize: 12
      horizontalAlignment: TextInput.AlignRight
      verticalAlignment: TextInput.AlignVCenter
      Layout.fillWidth: true
      Layout.fillHeight: true
      background: Rectangle {
          color: "transparent"
      }
      placeholderText: qsTr("Search")
      onTextEdited: libraryModel.filterPattern = text
    }
    Text {
      id: searchIcon
      Layout.alignment: Qt.AlignCenter
      Layout.maximumWidth: 50
      Layout.fillWidth: true
      Layout.fillHeight: true
      font.family: "Material Design Icons"
      font.pixelSize: 20
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
      color: "#F5F5F5"
      text: MdiFont.Icon.magnify
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
