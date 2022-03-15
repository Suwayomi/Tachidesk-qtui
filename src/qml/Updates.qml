import QtQuick 2.8
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0

import Tachidesk.Models 1.0
import SortFilterProxyModel 0.2

Rectangle {

  color: "#333333"

  UpdatesModel {
    id: updatesModel
    nm: networkManager
  }

  function markRead(mangaId, chapter) {
    updatesModel.chapterRead(mangaId, chapter)
  }

  ProgressBar {
    id: progressbar
    anchors {
      top: parent.top
      left: parent.left
      right: parent.right
      margins: 8
    }
    value: updatesModel.complete / updatesModel.total
    visible: updatesModel.running
  }

  ListView {
    id: downloadView
    clip: true
    anchors {
      left: parent.left
      right: parent.right
      bottom: parent.bottom
      top: progressbar.bottom
    }
    spacing: 4
    model: updatesModel

    property bool listviewLoaded: false
    Component.onCompleted: {
      listviewLoaded = true
    }
    onAtYEndChanged: {
      if (!listviewLoaded) {
        return
      }
      updatesModel.next()
    }
    delegate: Item {
      id: item
      height: 100
      width: downloadView.width

      MouseArea {
        anchors.fill: parent
        onClicked: {
          var viewer = navigatePage(Qt.resolvedUrl("WebtoonViewer.qml"),
                        { mangaNumber: mangaId,
                          chapter:     chapterIndex  })
          viewer.chapterRead.connect(markRead)
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
              //details.mangaChanged.connect(markRead)
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
            color: "white"
          }
          Text {
            Layout.fillWidth: true
            width: parent.width
            text: title
            font.bold: true
            font.pixelSize: 20
            fontSizeMode: Text.Fit
            color: read ? "grey" : "white"
          }
          Text {
            Layout.fillWidth: true
            width: parent.width
            text: name
            fontSizeMode: Text.Fit
            color: "white"
          }
        }

        Item {
          height: parent.height
          width: 30
          Rectangle {
            radius: 15
            color: "#0492c2"
            width: 30
            height: 30
            anchors.centerIn: parent
            visible: downloadProgress < 0 || downloadProgress >= 100
            Text {
              text: downloaded ? "✅" : "⬇"
              color: "white"
              anchors.centerIn: parent
              font.bold: true
              font.pixelSize: 20
              fontSizeMode: Text.Fit
            }
            MouseArea {
              anchors.fill: parent
              onClicked: updatesModel.downloadChapter(index)
            }
          }
          RadialBarShape {
            height: parent.height
            width: parent.width
            visible: downloadProgress > 0 && downloadProgress < 100
            progressColor: "#e6436d"
            value: downloadProgress
            spanAngle: 270
            dialType: RadialBarShape.DialType.FullDial
            backgroundColor: "#6272a4"
            penStyle: Qt.FlatCap
            dialColor: "transparent"
          }
        }
      }
    }

    PullToRefreshHandler {
      id: pulldown_handler
      threshold: 50
      onPulldownrelease: {
        updatesModel.refresh()
      }
    }
  }
}
