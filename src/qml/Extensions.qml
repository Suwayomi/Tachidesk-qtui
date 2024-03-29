import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tachidesk.Qtui
import SortFilterProxyModel
import "../../libs/QmlBridgeForMaterialDesignIcons/Icon.js" as MdiFont

Rectangle {

  color: "#212121"

  ExtensionModel {
    id: extensionModel
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

  RowLayout {
      id: searchField
      height: 50
      width: parent.width
      spacing: 0
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
      TextField {
        id: searchBox
        leftPadding: 0
        color: "#F5F5F5"
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
        color: "#F5F5F5"
      }
      Text {
        text: lang
        color: "#F5F5F5"
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
