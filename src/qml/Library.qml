import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Tachidesk.Qtui
import SortFilterProxyModel

import "../../libs/QmlBridgeForMaterialDesignIcons/Icon.js" as MdiFont

Item {
  id: libraryBase
  SortFilterProxyModel {
    id: libraryModel
    sourceModel: library
    filterRoleName: "title"
    filterCaseSensitivity: Qt.CaseInsensitive
    sorters: [
      RoleSorter {
        id: recentlyReadSorter
        roleName: "recentlyRead"
        sortOrder: Qt.DescendingOrder
      },
      RoleSorter {
        id: titleSorter
        roleName: "title"
        sortOrder: Qt.AscendingOrder
      }
    ]
  }

  LibraryModel {
    id: library
  }

  function refresh() {
    library.refreshLibrary()
  }

  Rectangle {
    anchors.fill: parent
    color: "#424242"
  }

  Text {
    id: title
    anchors {
      top: parent.top
      left: parent.left
      margins: 4
    }
    color: "#F5F5F5"
    text: qsTr("Library")
    font.pixelSize: 24
    font.bold: true
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    fontSizeMode: Text.Fit
  }

  Popup {
    id: optionsPopup
    width: parent.width * .80
    height: parent.height * .25
    focus: true
    modal: true
    anchors.centerIn: parent

    Rectangle {
      anchors.fill: parent
      color: "#212121"
      ComboBox {
        id: sortBy
        anchors.fill: parent
        model: [
          qsTr("Recently Read"),
          qsTr("Title")
        ]
        currentIndex: 0
        onActivated: (index) => {
          if ((index === 0 && libraryModel.sorters[0].roleName === "recentlyRead") || (index === 1 && libraryModel.sorters[0].roleName === "title")) {
            libraryModel.sorters[0].sortOrder = !libraryModel.sorters[0].sortOrder
            optionsPopup.close()
            return
          }

          index ?
            libraryModel.sorters = [ titleSorter, recentlyReadSorter ] :
            libraryModel.sorters = [ recentlyReadSorter, titleSorter ]
          optionsPopup.close()
        }
      }
    }
  }

  Rectangle {
    id: sortSelector
    width: parent.width / 3
    height: 40
    anchors {
      top: parent.top
      horizontalCenter: parent.horizontalCenter
    }
    color: "#212121"

    Text {
      id: infoText
      anchors {
        centerIn: parent
        margins: 5
      }
      color: "#F5F5F5"
      font.pixelSize: 14
      //font.weight: Font.DemiBold
      wrapMode: Text.WordWrap
      maximumLineCount: 3
      text: qsTr("Sort by")
      style: Text.Outline
      styleColor: "black"
    }

    MouseArea {
      anchors.fill: parent
      onClicked: {
        optionsPopup.open()
      }
    }
  }


  RowLayout {
    id: searchField
    height: 40
    width: parent.width / 3
    anchors {
      top: parent.top
      left: sortSelector.right
      leftMargin: 8
      right: parent.right
    }
    spacing: 0
    TextField {
      id: searchBox
      leftPadding: 0
      color: "#F5F5F5"
      font.pointSize: 12
      horizontalAlignment: TextInput.AlignRight
      verticalAlignment: TextInput.AlignVCenter
      Layout.fillWidth: true
      Layout.fillHeight: true
      background: Rectangle {
          color: "transparent"
      }
      placeholderText: qsTr("Search")
      onTextEdited: libraryModel.filterPattern = text
    }
    Text {
      id: searchIcon
      Layout.alignment: Qt.AlignCenter
      Layout.maximumWidth: 50
      Layout.fillWidth: true
      Layout.fillHeight: true
      font.family: "Material Design Icons"
      font.pixelSize: 20
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
      color: "#F5F5F5"
      text: MdiFont.Icon.magnify
    }
  }


  LibraryBase {
    anchors {
      top: title.bottom
      left: parent.left
      right: parent.right
      bottom: parent.bottom
    }
  }
}
