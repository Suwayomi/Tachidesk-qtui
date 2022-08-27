import QtQuick 2.8


Item {

  function reload() {
    stack.source = ""
    app.reload()
    stack.source = Qt.resolvedUrl("NavigationStack.qml")
  }

  function back() {
    stack.item.depth() <= 1 ? stack.item.canClose = true : stack.item.canClose = false
    stack.item.pop()
    stack.item.depth() > 1 ? stack.item.navigationVisible = false : stack.item.navigationVisible = true
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
