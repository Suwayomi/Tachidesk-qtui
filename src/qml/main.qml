import QtQuick
import QtQuick.Window
import QtQuick.Controls.Material

ApplicationWindow {
  id: root

  // Application main font
  font.family: "Roboto"//Settings.font || appFont.name

  height: 800
  width: 400
  visible: true

  /*
   * @param {number} changeTo - change from full screen to windowed and visa versa
  */
  function changeVisiblity(changeTo : int) {
    if (changeTo === Window.FullScreen) {
      root.showFullScreen()
    }
    else {
      root.showNormal()
    }
  }

  // Style settings
  Material.theme: Material.Dark //settings.lightTheme ? Material.Light : Material.Dark

  title: "Tachidesk Qtui"

  property variant rootWindow: root

  onClosing: (close) => {
    if (Qt.platform.os !== "android" || ac.canExit()) {
      close.accepted = true
      Qt.quit()
      return
    }
    close.accepted = false
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

