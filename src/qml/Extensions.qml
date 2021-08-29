import QtQuick 2.8
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0

import Tachidesk.Models 1.0
import SortFilterProxyModel 0.2

Item {
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
  }

  TextField {
    id: searchBox
    placeholderText: qsTr("Search")
    onAccepted: extensions.filterPattern = text
  }

  ListView {
    id: layout
    anchors {
      top: searchBox.bottom
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
      }
      Text {
        text: lang
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
