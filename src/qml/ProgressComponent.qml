import QtQuick 2.15
import QtQuick.Controls 2.15
import "../../libs/QmlBridgeForMaterialDesignIcons/Icon.js" as MdiFont

Item {
    id: root
    
    width: 40
    height: 40
    
    property real progress: 0
    property string state: "IDLE"  // IDLE, QUEUED, DOWNLOADING, FINISHED, ERROR
    property bool preparingForDownload: false
    
    signal downloadRequested()
    signal viewRequested()
    
    Rectangle {
        id: downloadButton
        anchors.fill: parent
        radius: width/2
        color: {
            switch(state) {
                case "FINISHED": return "#4CAF50"  // Green for completed
                case "ERROR": return "#F44336"     // Red for error
                case "DOWNLOADING": return "#2196F3" // Blue for active
                default: return "#0492c2"          // Default blue
            }
        }
        
        Text {
            anchors.centerIn: parent
            text: {
                switch(state) {
                    case "FINISHED": return MdiFont.Icon.checkCircle
                    case "ERROR": return MdiFont.Icon.alertCircle
                    case "DOWNLOADING": return MdiFont.Icon.download
                    default: return MdiFont.Icon.downloadCircleOutline
                }
            }
            font.family: "Material Designer Icons"
            color: "white"
            font.pixelSize: parent.width * 0.6
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        
        RadialBarShape {
            anchors.fill: parent
            visible: state === "DOWNLOADING" || preparingForDownload
            value: progress
            progressColor: "#e6436d"
            backgroundColor: "transparent"
            spanAngle: 270
            dialType: RadialBarShape.DialType.FullDial
            penStyle: Qt.FlatCap
        }
        
        MouseArea {
            anchors.fill: parent
            onClicked: {
                switch(state) {
                    case "IDLE":
                    case "QUEUED":
                        root.downloadRequested()
                        break
                    case "FINISHED":
                        root.viewRequested()
                        break
                }
            }
        }
    }
}