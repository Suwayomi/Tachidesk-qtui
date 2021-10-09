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

  ListView {
    id: downloadView
    clip: true
    anchors.fill: parent
    spacing: 4
    model: updatesModel
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
        }
      }
    }
  }
}
