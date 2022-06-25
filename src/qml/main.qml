import QtQuick 2.8
import QtQuick.Window 2.2
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material
import Qt.labs.settings 1.0 as Labs

ApplicationWindow {
  id: root

  // Application main font
  font.family: "Roboto"//Settings.font || appFont.name

  height: 800
  width: 400
  visible: true

  function changeVisiblity(changeTo) {
      console.log("root visible: ", changeTo)
    //root.visibility = changeTo
    if (changeTo == Window.FullScreen) {
        console.log("\n\n\n fullscreenQ!!!")
        root.showFullScreen()
    }
    else {
        console.log("\n\n\n normal!!!")
        root.showNormal()
    }
  }

  // Style settings
  Material.theme: settings.lightTheme ? Material.Light : Material.Dark

  title: "Tachidesk Qtui"

  property variant rootWindow: root
  function isMobile() {
      return {android: true, ios: true, winphone: true}[Qt.platform.os] || false;
  }

  onClosing: (close) => {
    if (Qt.platform.os != "android" || ac.canClose) {
      close.accepted = true
      Qt.quit()
    } else {
      close.accepted = false
    }
  }

  ApplicationContent {
    id: ac
    anchors.fill: parent
    focus: true

    Keys.onPressed: (event) => {
      switch (event.key) {
        case Qt.Key_R:
          if (event.modifiers & (Qt.ControlModifier | Qt.ShiftModifier)) {
            ac.reload()
          }
          break
        default:
          break
      }
    }

    Keys.onReleased: (event) => {
      switch (event.key) {
        case Qt.Key_Backspace:
          if (event.modifiers & Qt.ControlModifier)
          {
            ac.back()
          }
          break;
        case Qt.Key_Back:
          ac.back()
          break;
        default:
          break;
      }
    }
  }

}

