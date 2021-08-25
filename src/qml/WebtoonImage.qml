import QtQuick 2.8

Rectangle {
  color: "black"
  width: listView.width
  height: listView.width * 1.5

  Image {
    id: image
    fillMode: Image.PreserveAspectFit
    source: chapterUrl
    width: parent.width
    // this is only needed for resizing windows
    onWidthChanged: if (paintedHeight != 0) parent.height = paintedHeight
    onStatusChanged: {
      if(image.status == Image.Ready ) {
        // max size the height can be
        parent.height = sourceSize.height
        // size the height will actually be
        parent.height = paintedHeight
      }
    }
  }
}