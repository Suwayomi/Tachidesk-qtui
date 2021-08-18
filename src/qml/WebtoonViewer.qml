import QtQuick 2.8
import QtQuick.Controls 2.15

Item {
  property string chapter
  property int pagecount

  ListView {
    id: listView
    anchors.fill: parent
    model: pagecount
    //cacheBuffer: 40
    delegate: Image {
      id: image
      width: listView.width
      fillMode: Image.PreserveAspectFit
      source: chapter + index
      onStatusChanged: {
        //if (image.status == Image.Loading) height = 500
        //else if (image.status == Image.Ready) {
        //  height = sourceSize.height
        //  width = listView.width
        //}
      }
    }
  }
}
