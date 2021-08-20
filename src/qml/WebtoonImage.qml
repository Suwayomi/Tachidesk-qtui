import QtQuick 2.8

Rectangle {
  color: "black"
  width: listView.width
  height: listView.width * 1.5

  Image {
    id: image
    //anchors.fill: parent
    fillMode: Image.PreserveAspectFit
    source: chapter + index
    width: parent.width
    onWidthChanged: if (paintedHeight != 0) parent.height = paintedHeight
    onStatusChanged: {
      if(image.status == Image.Ready ) {
        parent.height = sourceSize.height
        parent.height = paintedHeight
        //console.log(paintedWidth, sourceSize.width, width)
      }
      //if(image.status == Image.Ready ) {
      //  parent.height = sourceSize.height
      //  //height = sourceSize.height
      //}
      //if (image.status == Image.Loading) sourceSize.width = parent.width
      //else if (image.status == Image.Ready) {
      //  height = sourceSize.height
      //  width = listView.width
      //}
    }
  }
}
