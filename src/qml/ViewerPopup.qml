import QtQuick
import QtQuick.Controls

Popup {
  id: settingsPopup
  width: parent.width * .80
  height: 75
  focus: true
  modal: true
  anchors.centerIn: parent
  enter: Transition {
    NumberAnimation { property: "opacity"; from: 0.0; to: 1.0 }
  }
  exit: Transition {
    NumberAnimation { property: "opacity"; from: 1.0; to: 0.0 }
  }

  Rectangle {
    anchors.fill: parent
    color: "#212121"

    ComboBox {
      anchors.centerIn: parent
      width: parent.width * .85
      height: parent.height * .85
      model: [ "WebToon", "Manga" ]
      currentIndex: settings.viewer
      onActivated: (index) => {
        settings.viewer = index
        settingsPopup.close()
      }
    }
  }
}
