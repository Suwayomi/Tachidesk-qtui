import QtQuick 2.15

Item {
    id: root
    
    Column {
        anchors.fill: parent
        anchors.margins: 4
        spacing: 4
        
        // Manga cover placeholder
        LoadingPlaceholder {
            width: parent.width
            height: parent.height * 0.8
            radius: 4
        }
        
        // Title placeholder
        LoadingPlaceholder {
            width: parent.width * 0.7
            height: 12
            radius: 2
        }
    }
}