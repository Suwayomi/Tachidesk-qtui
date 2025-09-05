import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    
    property real pulseOpacity: 0.3
    property int pulseDuration: 1200
    
    color: "#3a3a3a"
    radius: 4
    
    SequentialAnimation on opacity {
        running: true
        loops: Animation.Infinite
        
        NumberAnimation {
            from: root.pulseOpacity
            to: root.pulseOpacity * 2
            duration: root.pulseDuration
            easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            from: root.pulseOpacity * 2
            to: root.pulseOpacity
            duration: root.pulseDuration
            easing.type: Easing.InOutQuad
        }
    }
}