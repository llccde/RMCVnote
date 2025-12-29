import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    property int count: 0
    property bool expanded: false
    
    height: 30
    color: expanded ? "#e0e0e0" : "#f0f0f0"
    radius: 4
    
    MouseArea {
        anchors.fill: parent
        onClicked: root.expanded = !root.expanded
        
        Row {
            anchors.centerIn: parent
            spacing: 5
            
            Text {
                text: root.expanded ? "▼" : "▶"
                font.pixelSize: 10
            }
            
            Text {
                text: root.expanded ? "收起" : ("+" + root.count + "条连续记录")
                font.pixelSize: 12
            }
        }
    }
}