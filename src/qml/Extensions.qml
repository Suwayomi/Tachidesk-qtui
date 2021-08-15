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

  ListView {
    id: layout
    anchors.fill: parent
    model: extensions

    delegate: RowLayout {
      width: layout.width
      Image {
        source: iconUrl
      }
      Text {
        text: name
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
