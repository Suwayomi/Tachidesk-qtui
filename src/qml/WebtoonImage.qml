import QtQuick 2.8
import QtQuick.Controls 2.15

Rectangle {
  color: "black"
  width: base.width
  height: base.width * 2
  signal imageLoaded()

  BusyIndicator {
    running: image.status == Image.Loading
    anchors.centerIn: parent
  }

  Image {
    id: image
    fillMode: Image.PreserveAspectFit
    source: chapterUrl
    width: parent.width
    cache: false
    // this is only needed for resizing windows
    onWidthChanged: if (paintedHeight != 0) parent.height = paintedHeight
    onStatusChanged: {
      if(image.status == Image.Ready ) {
        // max size the height can be
        parent.height = sourceSize.height
        // size the height will actually be
        parent.height = paintedHeight
        imageLoaded()
      }
    }
  }
}
