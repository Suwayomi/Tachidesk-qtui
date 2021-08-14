import QtQuick 2.8
import QtQuick.Controls 2.15

import Tachidesk.Models 1.0

Item {
  property alias mangaNumber: chaptersModel.mangaNumber

  ChaptersModel {
    id: chaptersModel
    nm: networkManager
  }

  ListView {
    anchors.fill: parent
    model: chaptersModel

    delegate: Button {
      width: parent.width
      height: 150
      text: qsTr("chapter: %1").arg(name)
      onClicked: {
        navigatePage(Qt.resolvedUrl("WebtoonViewer.qml"),
                                     { pagecount: pageCount,
                                         chapter: chapterUrl })
      }
    }
  }
}
