import QtQuick 2.8
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.15

import Tachidesk.Models 1.0

Item {
  property alias source: libraryModel.source
  SourcesLibraryModel {
    id: libraryModel
    nm: networkManager
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
      color: "#333333"
      RowLayout {
          anchors.fill: searchField
          spacing: 0
          Image {
              id: searchIcon
              Layout.alignment: Qt.AlignCenter
              Layout.maximumWidth: 50
              Layout.fillWidth: true
              Layout.fillHeight: true
              scale: 0.5
              fillMode: Image.PreserveAspectFit
              source: "./../../images/outline_search_white_24dp.png"
          }
          TextField {
            id: search
            leftPadding: 0
            color: "white"
            font.pointSize: 12
            Layout.fillWidth: true
            Layout.fillHeight: true
            background: Rectangle {
                color: "transparent"
            }
            placeholderText: "search"
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
