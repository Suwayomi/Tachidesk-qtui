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

      RowLayout {
        anchors {
          fill: parent
          margins: 4
        }
        Image {
          Layout.fillWidth: true
          sourceSize.height: item.height * .80
          source: thumbnailUrl
          fillMode: Image.PreserveAspectFit
        }

        ColumnLayout {
          Layout.fillWidth: true
          Text {
            Layout.fillWidth: true
            width: parent.width
            text: title
            font.bold: true
            font.pixelSize: 20
            fontSizeMode: Text.Fit
            color: "white"
          }
          Text {
            Layout.fillWidth: true
            width: parent.width
            text: name
            fontSizeMode: Text.Fit
            color: "white"
          }
        }

        Rectangle {
          Layout.minimumWidth: 30
          Layout.minimumHeight: 30
          radius: 15
          color: "#0492c2"
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
