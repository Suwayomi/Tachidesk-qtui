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
    delegate: Item {
      Column {
        id: mangaInfo
        spacing: 8
        anchors.fill: parent
        Image {
          id: image
          source: thumbnailUrl
          fillMode: Image.PreserveAspectFit
          width: layout.cellWidth
          sourceSize.height: 300
          MouseArea {
            anchors.fill: parent
            onClicked: {
              navigatePage(Qt.resolvedUrl("MangaDetails.qml"), { mangaNumber: mangaId })
            }
          }
        }
        Text {
          Layout.alignment: Qt.AlignLeft
          horizontalAlignment: Text.AlignLeft
          text: title
        }
      }

    }
  }
}
