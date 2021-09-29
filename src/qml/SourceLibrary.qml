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

  TextField {
    id: search
    padding: 8
    anchors {
      left: parent.left
      right: parent.right
      top: parent.top
    }
    height: 50
    color: "white"
    font.pointSize: 12
    background: Rectangle {
        color: "#333333"
    }
    placeholderText: "   search"
    onAccepted: libraryModel.search(search.text)
  }

  LibraryBase {
    anchors {
      top: search.bottom
      left: parent.left
      right: parent.right
      bottom: parent.bottom
    }
    onReachedEnd: libraryModel.next()
  }
}
