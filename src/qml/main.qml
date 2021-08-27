import QtQuick 2.8
import QtQuick.Window 2.2
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import Qt.labs.settings 1.0 as Labs

ApplicationWindow {
  id: root

  property alias app: root

  // Application main font
  //font.family: Settings.font || appFont.name

  height: 800
  width: 400
  //minimumHeight: 480
  //minimumWidth: 480
  visible: true

  //Labs.Settings {
  //    id: windowSettings
  //    property var windowX
  //    property var windowY
  //    property var windowWidth
  //    property var windowHeight
  //    property var windowVisibility
  //    function update() {
  //        Qt.callLater(function() {
  //            if (visibility === Window.Windowed) {
  //                windowX = x
  //                windowY = y
  //                windowWidth = width
  //                windowHeight = height
  //            }
  //            windowVisibility = visibility
  //        })
  //    }
  //    Component.onCompleted: {
  //        if (isMobile()) return
  //        if (visibility === Window.Windowed) {
  //            x = windowX !== undefined ? windowX : x
  //            y = windowY !== undefined ? windowY : y
  //            width = windowWidth !== undefined ? windowWidth : width
  //            height = windowHeight !== undefined ? windowHeight : height
  //            visibility = windowVisibility === Window.Maximized ? windowVisibility : visibility
  //        }
  //        root.onXChanged.connect(update)
  //        root.onYChanged.connect(update)
  //        root.onWidthChanged.connect(update)
  //        root.onHeightChanged.connect(update)
  //        root.onVisibilityChanged.connect(update)
  //    }
  //}

  // Style settings
  //Material.theme: Settings.lightTheme ? Material.Light : Material.Dark

  title: "Tachidesk-qtui"

  //visibility: (!isMobile() && Settings.minimizeOnStartup) ? Window.Minimized : Window.AutomaticVisibility

  property int restoredVisibility: Window.AutomaticVisibility
  //onAppFullScreenChanged: {
  //    if (visibility != Window.FullScreen) {
  //        restoredVisibility = visibility
  //    }
  //    visibility = appFullScreen ? Window.FullScreen : restoredVisibility
  //}

  property variant rootWindow: root
  property variant g_tooltip
  property bool g_contextMenuVisible: false
  //property bool appFullScreen: isMobile() ? (view.playerVisible && !isPortraitMode) : false
  property bool isPortraitMode: Screen.primaryOrientation === Qt.PortraitOrientation
                                || Screen.primaryOrientation === Qt.InvertedPortraitOrientation

  //function fitToAspectRatio() {
  //    height = Math.floor(view.width * 0.5625)
  //}

  function isMobile() {
      return {android: true, ios: true, winphone: true}[Qt.platform.os] || false;
  }

  onClosing: {
    if (ac.canClose) {
      close.accepted = true
      Qt.quit()
    } else {
      close.accepted = false
    }
  }

  ApplicationContent {
    id: ac
    anchors.fill: parent
  }

  //FontLoader {
  //    source: "fonts/MaterialIcons-Regular.ttf"
  //    name: "Material Icons"
  //}

  //FontLoader {
  //    id: appFont

  //    source: "fonts/NotoSans-Regular.ttf"
  //    name: "Noto Sans"
  //}
}

