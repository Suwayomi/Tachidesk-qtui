import QtQuick
import QtQuick.Controls

Item {
  property bool canClose: false
  property bool navigationVisible: true

  function changeWindowVisibility(changeTo) {
    root.changeVisiblity(changeTo)
  }

  function pop() {
    stack.pop()
  }

  function navigateHome() {
    if (stack.currentItem.url.toString() !== stack.initialItem.toString()) {
      stack.replace(null, stack.initialItem, {
                      "url": stack.initialItem
                    })
    }
  }

  function navigatePage(source, properties) {
    canClose = false
    if (stack.currentItem && stack.currentItem.url
      && Qt.resolvedUrl(stack.currentItem.url) === Qt.resolvedUrl(source))
    {
      for (var i in properties) {
        if (i !== "replace") {
          stack.currentItem[i] = properties[i]
        }
      }
    }

    var item = stack.find(function(item, index) {
      if (!item.url) {
        return false
      }
      console.log("item url", item.url, source)
      return Qt.resolvedUrl(item.url) === Qt.resolvedUrl(source)
    })

    if (item) {
      stack.pop(item, StackView.Immediate)
      return
    }

    properties["url"] = source

    if (properties["replace"]) {
      stack.replace(Qt.resolvedUrl(source), properties)
      navigationVisible = true
    }
    else {
      navigationVisible = false
      return stack.push(Qt.resolvedUrl(source), properties)
    }
  }

  Connections {
    target: stack.currentItem

  }

  StackView {
    id: stack
    anchors {
      left: parent.left
      right: parent.right
      bottom: navigation.visible ? navigation.top : parent.bottom
      top: parent.top
    }

    Component.onCompleted: stack.push(Qt.resolvedUrl("Library.qml"))

    pushEnter: Transition {
      PropertyAnimation {
        property: "opacity"
        from: 0
        to: 1
        duration: 200
      }
    }
    pushExit: Transition {
      PropertyAnimation {
        property: "opacity"
        from: 1
        to:0
        duration: 200
      }
    }
    popEnter: Transition {
        PropertyAnimation {
          property: "opacity"
          from: 0
          to:1
          duration: 200
        }
    }
    popExit: Transition {
        PropertyAnimation {
          property: "opacity"
          from: 1
          to:0
          duration: 200
        }
    }
    replaceEnter: Transition {
      PropertyAnimation {
        property: "opacity"
        from: 0
        to:1
        duration: 200
      }
    }
    replaceExit: Transition {
      PropertyAnimation {
        property: "opacity"
        from: 1
        to:0
        duration: 200
      }
    }
  }

  Rectangle {
    id: navigation
    visible: navigationVisible
    color: "#333333"
    anchors {
      left: parent.left
      right: parent.right
      bottom: parent.bottom
    }
    height: 50
    NavigationHome {
      anchors.fill: parent
    }
  }

  // Swipe back to navigate
  BackGestureDetector {
    allowedToWork: stack.depth > 1
    onBackGestureDetected: {
      stack.pop()
      stack.depth > 1 ? navigationVisible = false : navigationVisible = true
    }

    anchors {
        top: parent.top
        left: parent.left
        bottom: parent.bottom
    }
  }
}
