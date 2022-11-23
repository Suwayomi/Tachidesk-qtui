import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Tachidesk.Qtui
import "../../libs/QmlBridgeForMaterialDesignIcons/Icon.js" as MdiFont

Item {
  property alias source: libraryModel.source
  SourcesLibraryModel {
    id: libraryModel
  }

  Rectangle {
      id: searchField
      anchors {
        left: parent.left
        right: parent.right
        top: parent.top
      }
      width: parent.width
      height: 50
      color: "#212121"
      RowLayout {
          anchors.fill: searchField
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
            id: search
            leftPadding: 0
            color: "#F5F5F5"
            font.pointSize: 12
            Layout.fillWidth: true
            Layout.fillHeight: true
            background: Rectangle {
                color: "transparent"
            }
            placeholderText: "Search"
            onAccepted: libraryModel.search(search.text)
          }
      }
  }

  LibraryBase {
    anchors {
      top: searchField.bottom
      left: parent.left
      right: parent.right
      bottom: parent.bottom
    }
    onReachedEnd: libraryModel.next()
  }
}
