import QtQuick
import QtQuick.Controls

Rectangle {
  color: "black"
  width: base.width
  height: base.width * 2
  property int timeoutMs: 5000
  property int imageIndex
  property real imageHeight: image.height
  property bool imageTimeout: false
  signal imageLoaded()

  BusyIndicator {
    running: image.status === Image.Loading
    anchors.centerIn: parent
  }

  Timer {
    id: loadTimer
    interval: imageIndex * 100   // 0.2 seconds
    repeat: false
    onTriggered: image.source = "image://network/" + chapterUrl
  }

  Component.onCompleted: loadTimer.start()

  Image {
    id: image
    fillMode: Image.PreserveAspectFit
    // source: visible ? chapterUrl : ""
    width: parent.width
    cache: true
    // this is only needed for resizing windows
    onWidthChanged: if (paintedHeight !== 0) parent.height = paintedHeight
    onStatusChanged: {
      if (image.Status == Image.Error) {
        imageTimeout = true;
        console.log("Image load error:", chapterUrl, image.errorString);
      }
      if(image.status === Image.Ready ) {
        // max size the height can be
        parent.height = sourceSize.height
        // size the height will actually be
        parent.height = paintedHeight
        imageLoaded()
      }
    }
  }

  // Timeout fallback
  Timer {
    id: timeoutTimer
    interval: timeoutMs
    running: image.status === Image.Loading
    repeat: false

    onTriggered: {
      console.warn("Image load timed out:", chapterUrl);
      imageTimeout = true
      image.source = ""; // Optionally clear it to force retry later
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
    visible: image.status === Image.Error || imageTimeout
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
        image.source = "image://network/" + chapterUrl + "?cacheBust=" + Date.now();
        imageTimeout = false; // Reset the timeout flag
      }
    }
  }
}
