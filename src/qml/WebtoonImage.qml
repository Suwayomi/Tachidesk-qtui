import QtQuick
import QtQuick.Controls

Rectangle {
  color: "black"
  width: base.width
  height: base.width * 2
  property real imageHeight: image.height
  signal imageLoaded()

  BusyIndicator {
    running: image.status === Image.Loading
    anchors.centerIn: parent
  }

  Image {
    id: image
    fillMode: Image.PreserveAspectFit
    source: chapterUrl
    width: parent.width
    cache: true
    // this is only needed for resizing windows
    onWidthChanged: if (paintedHeight !== 0) parent.height = paintedHeight
    onStatusChanged: {
      if(image.status === Image.Ready ) {
        // max size the height can be
        parent.height = sourceSize.height
        // size the height will actually be
        parent.height = paintedHeight
        imageLoaded()
      }
    }
  }

  Rectangle {
    id: retry
    anchors.centerIn: parent
    width: 100
    height: 50
    color: "gray"
    radius: 4
    border.color: "#0492c2"
    border.width: 1
    visible: image.status === Image.Error
    Text {
      anchors.centerIn: parent
      text: qsTr("Retry")
      font.pixelSize: 18
      color: "#F5F5F5"
    }
    z: 5

    MouseArea {
      anchors.fill: parent
      onClicked: {
        image.source = "";  // Clear first (optional, for full reset)
        image.source = chapterUrl + "?cacheBust=" + Date.now();
      }
    }
  }
}
