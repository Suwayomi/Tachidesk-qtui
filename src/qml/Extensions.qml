import QtQuick 2.8
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0

import Tachidesk.Models 1.0
import SortFilterProxyModel 0.2

Rectangle {

  color: "#333333"

  ExtensionModel {
    id: extensionModel
    nm: networkManager
  }

  SortFilterProxyModel {
    id: extensions
    sourceModel: extensionModel
    filterCaseSensitivity: Qt.CaseInsensitive
    filterRoleName: "name"

    sorters: [
      RoleSorter {
        roleName: "installed"
        sortOrder: Qt.DescendingOrder
      },
      RoleSorter {
        roleName: "hasUpdate"
        sortOrder: Qt.DescendingOrder
      }
    ]
    filters: AnyOf {
      ValueFilter {
        roleName: "isNsfw"
        value: settings.nsfw
      }
      ValueFilter {
        roleName: "isNsfw"
        value: false
      }
    }
  }

  RowLayout{
      id: searchField
      height: 50
      width: parent.width
      spacing: 0
      Image {
          id: searchIcon
          Layout.alignment: Qt.AlignCenter
          Layout.maximumWidth: 50
          Layout.fillWidth: true
          Layout.fillHeight: true
          scale: 0.5
          fillMode: Image.PreserveAspectFit
          source: Qt.resolvedUrl("images/outline_search_white_24dp.png")
      }
      TextField {
        id: searchBox
        leftPadding: 0
        color: "white"
        font.pointSize: 12
        Layout.fillWidth: true
        Layout.fillHeight: true
        background: Rectangle {
            color: "transparent"
        }
        placeholderText: qsTr("Search")
        onAccepted: extensions.filterPattern = text
      }
  }


  ListView {
    id: layout
    anchors {
      top: searchField.bottom
      right: parent.right
      left: parent.left
      bottom: parent.bottom
    }
    model: extensions
    clip: true

    delegate: RowLayout {
      width: layout.width
      Image {
        source: iconUrl
      }
      Text {
        Layout.alignment: Qt.AlignLeft
        horizontalAlignment: Text.AlignLeft
        text: name
        color: "white"
      }
      Text {
        text: lang
        color: "white"
      }
      Button {
        Layout.alignment: Qt.AlignRight
        Layout.rightMargin: 4

        text: installed ? hasUpdate ? "update" : "uninstall" : "install"
        onClicked: {
          if (hasUpdate) {
            extensionModel.update(pkgName, row)
          } else if (installed) {
            extensionModel.uninstall(pkgName, row)
          } else {
            extensionModel.install(pkgName, row)
          }
        }
      }
    }
  }
}
