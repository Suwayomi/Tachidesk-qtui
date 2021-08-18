import QtQuick 2.8
import QtQuick.Layouts 1.0

import Tachidesk.Models 1.0

Item {
  property alias source: sourcesModel.source
  SourcesLibraryModel {
    id: sourcesModel
    nm: networkManager
  }

  GridView {
    id: layout
    anchors.fill: parent
    clip: true
    model: sourcesModel
    cellWidth: 200
    cellHeight: 324
    delegate: Column {
      spacing: 8
      Image {
        id: image
        source: thumbnailUrl
        fillMode: Image.PreserveAspectFit
        width: 200
        sourceSize.height: 300
      }
      Text {
        Layout.alignment: Qt.AlignLeft
        horizontalAlignment: Text.AlignLeft
        text: title
      }
    }
  }
}
