import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tachidesk.Qtui
import SortFilterProxyModel

import "../../libs/QmlBridgeForMaterialDesignIcons/Icon.js" as MdiFont

Rectangle {

  property bool isLoading: updatesModel.rowCount() === 0 || updatesModel.running
  Connections {
    target: updatesModel
    function onModelReset() {
      console.log("updates model reset, isLoading:", updatesModel.rowCount() === 0)
      isLoading = updatesModel.rowCount() === 0 || updatesModel.running
    }
  }

  color: "#212121"

  UpdatesModel {
    id: updatesModel
  }

  function refresh() {
    updatesModel.pageRefresh()
  }

  function markRead(chapterId, chapter) {
    console.log("marking chapter as read:", chapterId, chapter)
    updatesModel.chapterRead(chapterId, chapter)
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
    model: isLoading ? 8 : updatesModel

    property bool listviewLoaded: false
    Component.onCompleted: {
      listviewLoaded = true
    }
    onAtYEndChanged: {
      if (!listviewLoaded || isLoading) {
        return
      }
      updatesModel.next()
    }
    delegate: isLoading ? placeholderDelegate : updateDelegate
  }

  Component {
    id: placeholderDelegate
    LibraryCardPlaceholder {
      height: 100
      width: downloadView.width
    }
  }

  Component {
    id: updateDelegate
    Item {
      id: item
      height: 100
      width: downloadView.width

      MouseArea {
        anchors.fill: parent
        onClicked: {
          var viewer = navigatePage(Qt.resolvedUrl("Viewer.qml"),
                        { mangaNumber: mangaId,
                        chapter:     chapterIndex,
                        chapterId:   chapterId })
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
            source: "image://network/" + thumbnailUrl
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
            text: "%1".arg(Qt.formatDate(new Date(fetchedAt * 1000), "MMM dd"))
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

        ProgressComponent {
          width: 30
          height: 30
          progress: downloadProgress
          preparingForDownload: downloadPrepairing
          state: {
            if (downloaded) return "FINISHED"
            if (downloadPrepairing) return "QUEUED"
            if (downloadProgress > 0 && downloadProgress < 100) return "DOWNLOADING"
            return "IDLE"
          }

          onDownloadRequested: {
            console.log("Download requested for chapter:", chapterId, "at index:", index)
            updatesModel.downloadChapter(index)
          }

          onViewRequested: {
            // Open the chapter view
            var viewer = navigatePage(Qt.resolvedUrl("Viewer.qml"), {
              mangaNumber: mangaId,
              chapter:     chapterIndex,
              chapterId:   chapterId
            })
            viewer.chapterRead.connect(markRead)
          }
        }
      }
    }
  }

  PullToRefreshHandler {
    id: pulldown_handler
    onPulldownrelease: {
      updatesModel.refresh()
    }
  }
}
