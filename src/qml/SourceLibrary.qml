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
    anchors {
      left: parent.left
      right: parent.right
      top: parent.top
      margins: 4
    }
    height: 50
    placeholderText: "search"
    onAccepted: libraryModel.search(search.text)
  }

  LibraryBase {
    anchors {
      top: search.bottom
      topMargin: 4
      left: parent.left
      right: parent.right
      bottom: parent.bottom
    }
    onReachedEnd: libraryModel.next()
  }
}
