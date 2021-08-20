import QtQuick 2.8
import QtQuick.Controls 2.15

import Tachidesk.Models 1.0

Item {
  property alias mangaNumber: chapterModel.mangaNumber
  property alias chapterNumber: chapterModel.chapterNumber

  ChapterModel {
    id: chapterModel
    nm: networkManager
  }

  ListView {
    id: listView
    anchors.fill: parent
    model: chapterModel
    cacheBuffer: 10000
    delegate: Loader {
      source: "WebtoonImage.qml"
    }
  }
}
