import QtQuick

MouseArea {
  width: 32
  hoverEnabled: true
  scrollGestureEnabled: false
  propagateComposedEvents: true
  enabled: Qt.platform.os !== "android"

  signal backGestureDetected()

  property int xPrev: 0
  property int xStart: 0
  property real velocity: 0.0
  property bool tracing: false
  property bool allowedToWork: false

  onPressed: (mouse) => {
    if (allowedToWork) {
      xStart = mouse.x
      xPrev = mouse.x
      velocity = 0
      tracing = true
      mouse.accepted = true
    }

    else
      mouse.accepted = false
  }

  onPositionChanged: (mouse) => {
    if (!tracing) {
      mouse.accepted = false
      return
    }

    const currVel = (mouse.x - xPrev)
    velocity = (velocity + currVel)/2.0
    xPrev = mouse.x

    mouse.accepted = false
  }

  onReleased: (mouse) => {
    if (!tracing) {
      mouse.accepted = false
      return
    }

    tracing = false

    // velocity > 15 was original
    if (velocity > 0.9 && mouse.x > parent.width * 0.2) {
      backGestureDetected()
    }

    mouse.accepted = false
  }
}
