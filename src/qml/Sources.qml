import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tachidesk.Qtui
import SortFilterProxyModel
import "../../libs/QmlBridgeForMaterialDesignIcons/Icon.js" as MdiFont

Rectangle {

  color: "#212121"

  SourcesModel {
    id: sourcesModel
  }

  function refresh() {
    sourcesModel.refresh()
  }

  SortFilterProxyModel {
    id: sources
    sourceModel: sourcesModel
    filterCaseSensitivity: Qt.CaseInsensitive
    filterRoleName: "lang"
    filters: [
      ValueFilter {
        roleName: "lang"
        value: settings.lang
      }
    ]
  }

  RowLayout{
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
        text: MdiFont.Icon.filterOutline
      }
      TextField {
          id: searchBox
          leftPadding: 0
          visible: true
          color: "#F5F5F5"
          font.pointSize: 12
          Layout.fillWidth: true
        Layout.fillHeight: true
        background: Rectangle {
            color: "transparent"
        }
        placeholderText: qsTr("Language Filter")
        onAccepted: sources.filterPattern = text
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
    clip: true
    model: sources
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
        text: qsTr("Browse")
        onClicked: {
          navigatePage(Qt.resolvedUrl("SourceLibrary.qml"),
                                       { source: id })
        }
      }
    }
  }
}
