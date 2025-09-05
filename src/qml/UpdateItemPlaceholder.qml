import QtQuick 2.15

Item {
    id: root
    height: 100
    
    Row {
        anchors {
            fill: parent
            margins: 4
        }
        spacing: 4
        
        // Thumbnail placeholder
        LoadingPlaceholder {
            width: parent.height * 0.75
            height: parent.height
            radius: 4
        }
        
        Column {
            width: parent.width * 0.70
            height: parent.height
            spacing: 8
            anchors.verticalCenter: parent.verticalCenter
            
            // Date placeholder
            LoadingPlaceholder {
                width: parent.width * 0.3
                height: 16
                radius: 2
            }
            
            // Title placeholder
            LoadingPlaceholder {
                width: parent.width * 0.8
                height: 20
                radius: 2
            }
            
            // Manga name placeholder
            LoadingPlaceholder {
                width: parent.width * 0.6
                height: 14
                radius: 2
            }
        }
        
        // Download button placeholder
        LoadingPlaceholder {
            width: 30
            height: 30
            radius: 15
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}