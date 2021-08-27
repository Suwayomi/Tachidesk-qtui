import QtQuick 2.8
import QtQuick.Controls 2.15


Item {
  property bool canClose: false

  function navigateHome() {
    if (stack.currentItem.url.toString() !== stack.initialItem.toString()) {
      stack.replace(null, stack.initialItem, {
                      "url": stack.initialItem
                    }, StackView.Immediate)
    }
  }

  function navigatePage(source, properties) {
    canClose = false
    if (Qt.resolvedUrl(stack.currentItem.url) === Qt.resolvedUrl(source)) {
      for (var i in properties) {
        if (i !== "replace") {
          stack.currentItem[i] = properties[i]
        }
      }
    }

    var item = stack.find(function(item, index) {
      return Qt.resolvedUrl(item.url) === Qt.resolvedUrl(source)
    })

    if (item) {
      stack.pop(item, StackView.Immediate)
      return
    }

    properties["url"] = source

    if (properties["replace"]) {
      stack.replace(Qt.resolvedUrl(source), properties, StackView.Immediate)
    }
    else {
      return stack.push(Qt.resolvedUrl(source), properties, StackView.Immediate)
    }
  }

  //Connections {
  //  target: stack.currentItem

  //  function onBackClicked() {
  //    if (stack.depth === 1) {
  //      navigateHome()
  //    }
  //    else {
  //      stack.pop(StackView.Immediate)
  //    }
  //  }

  //  function onHomeClicked() {
  //    navigateHome()
  //  }

  //  function onChangeScreen(source, properties) {
  //    navigatePage(source, properties)
  //  }
  //}

  StackView {
    id: stack
    anchors.fill: parent
    initialItem: Qt.resolvedUrl("NavigationHome.qml")
  }

  // Swipe back to navigate
  BackGestureDetector {
    allowedToWork: stack.depth > 1
    onBackGestureDetected: stack.pop()

    anchors {
        top: parent.top
        left: parent.left
        bottom: parent.bottom
    }
  }

  focus: true
  Keys.onReleased: {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Backspace) {
      stack.depth <= 1 ? canClose = true : canClose = false
      stack.pop()
      console.log(canClose)
    }
  }
}
