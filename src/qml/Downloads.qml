import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tachidesk.Qtui
import SortFilterProxyModel
import CustomControls

Rectangle {

  color: "#212121"

  DownloadsModel {
    id: chapterInfoModel
  }

  RowLayout {
    id: topButtons
    height: 50
    Button {
      id: clear
      text: qsTr("clear")
      onClicked: chapterInfoModel.clear()
      width: parent.width
    }
    Button {
      id: pause
      text: qsTr("pause")
      onClicked: chapterInfoModel.pause()
      width: parent.width
    }
    Button {
      text: qsTr("start")
      onClicked: chapterInfoModel.start()
      width: parent.width
    }
  }

  ListView {
    anchors {
      top: topButtons.bottom
      right: parent.right
      left: parent.left
      bottom: parent.bottom
    }

    id: downloadView
    clip: true
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

        RadialBarShape {
          height: parent.height
          width: 30
          progressColor: "#e6436d"
          value: progress
          spanAngle: 270
          dialType: RadialBarShape.DialType.FullDial
          backgroundColor: "#6272a4"
          penStyle: Qt.FlatCap
          dialColor: "transparent"
        }
      }
    }
  }
}
