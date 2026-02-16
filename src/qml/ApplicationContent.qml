import QtQuick 2.8


Item {

  function reload() {
    stack.source = ""
    app.reload()
    stack.source = Qt.resolvedUrl("NavigationStack.qml")
  }

  function back() {
      console.log("going back, depth:", stack.item.depth())
    if (stack.item.depth() > 1) {
      console.log("popping item, depth > 1")
      stack.item.pop()
      stack.item.navigationVisible = false
    }
    else {
      console.log("cannot pop, depth = 1, setting canClose = true")
      stack.item.canClose = true
      stack.item.navigationVisible = true
    }
  }

  function canExit() {
    return stack.item.canClose
  }

  Loader {
    id: stack
    anchors.fill: parent
    sourceComponent: NavigationStack {}
  }

}
