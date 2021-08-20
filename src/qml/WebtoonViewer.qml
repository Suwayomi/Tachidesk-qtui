import QtQuick 2.8
import QtQuick.Controls 2.15

Item {
  property string chapter
  property int pagecount

  ListView {
    id: listView
    anchors.fill: parent
    model: pagecount
    cacheBuffer: 10000
    delegate: Loader {
      source: "WebtoonImage.qml"
    }
  }
}
