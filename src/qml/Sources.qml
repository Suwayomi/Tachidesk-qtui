import QtQuick 2.8
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0

import Tachidesk.Models 1.0
import SortFilterProxyModel 0.2

Rectangle {

  color: "#333333"

  SourcesModel {
    id: sourcesModel
    nm: networkManager
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

  TextField {
    id: searchBox
    placeholderText: qsTr("   Language Filter")
    onAccepted: sources.filterPattern = text
    width: parent.width
    height: 50
    color: "white"
    font.pointSize: 12
    background: Rectangle {
        color: "transparent"
    }
  }

  ListView {
    id: layout
    anchors {
      top: searchBox.bottom
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
        color: "white"
      }
      Text {
        text: lang
        color: "white"
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
