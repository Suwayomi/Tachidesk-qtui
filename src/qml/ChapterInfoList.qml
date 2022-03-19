import QtQuick 2.8
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0

import Tachidesk.Models 1.0
import SortFilterProxyModel 0.2

Item {

  ListView {
    id: downloadView
    clip: true
    anchors.fill: parent
    spacing: 4
    model: chapterInfoModel
    delegate: Item {
      id: item
      height: 100
      width: downloadView.width

      MouseArea {
        anchors.fill: parent
        onClicked: {
          navigatePage(Qt.resolvedUrl("WebtoonViewer.qml"),
                        { mangaNumber: mangaId,
                          chapter:     chapterIndex  })
        }
      }

      Row {
        anchors {
          fill: parent
          margins: 4
        }
        spacing: 4
        Item {
          width: parent.height * .75
          height: parent.height
          Image {
            source: thumbnailUrl
            fillMode: Image.PreserveAspectCrop
            anchors.fill: parent
            anchors.margins: 2
          }
          MouseArea {
            anchors.fill: parent
            onClicked: {
              var details = navigatePage(Qt.resolvedUrl("MangaDetails.qml"), { mangaNumber: mangaId })
              details.mangaChanged.connect(onMangaChanged)
            }
          }
        }

        ColumnLayout {
          width: parent.width * .70
          height: parent.height
          Text {
            Layout.fillWidth: true
            width: parent.width
            text: "%1".arg(Qt.formatDate(fetchedAt, "MMM dd"))
            font.pixelSize: 16
            fontSizeMode: Text.Fit
            color: "#F5F5F5"
          }
          Text {
            Layout.fillWidth: true
            width: parent.width
            text: title
            font.bold: true
            font.pixelSize: 20
            fontSizeMode: Text.Fit
            color: read ? "grey" : "#F5F5F5"
          }
          Text {
            Layout.fillWidth: true
            width: parent.width
            text: name
            fontSizeMode: Text.Fit
            color: "#F5F5F5"
          }
        }

        Component {
          id: buttonSource
          RadialBarShape {
            progressColor: "#e6436d"
            Component.onCompleted: console.log("index?", index, downloadView.get(index))
            value: downloadView.get(index).progress
            spanAngle: 270
            dialType: RadialBarShape.DialType.FullDial
            backgroundColor: "#6272a4"
            penStyle: Qt.FlatCap
            dialColor: "transparent"
          }
        }

        Loader {
          id: buttonLoader
          height: parent.height
          width: 30
          sourceComponent: buttonSource
          onLoaded: console.log("progress", buttonLoader.item.progress)
        }

        //Item {
        //  height: parent.height
        //  width: 30
        //  Rectangle {
        //    radius: 15
        //    color: "#0492c2"
        //    width: 30
        //    height: 30
        //    anchors.centerIn: parent
        //    Text {
        //      text: downloaded ? "✅" : "⬇"
        //      color: "#F5F5F5"
        //      anchors.centerIn: parent
        //      font.bold: true
        //      font.pixelSize: 20
        //      fontSizeMode: Text.Fit
        //    }
        //    MouseArea {
        //      anchors.fill: parent
        //      onClicked: chapterInfoModel.downloadChapter(index)
        //    }
        //  }
        //}
      }
    }
  }
}
