import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    property int sortType: 0
    property bool isSelected: false
    property string text: ""
    
    signal sortRequested(int sortType)
    signal clicked()
    
    color: isSelected ? "#2196F3" : (mouseArea.containsMouse ? "#E3F2FD" : "transparent")
    radius: 4
    border.color: isSelected ? "#1976D2" : "#BDBDBD"
    border.width: 1
    
    implicitHeight: 36
    implicitWidth: textItem.implicitWidth + 20
    
    Text {
        id: textItem
        anchors.centerIn: parent
        text: root.text
        color: root.isSelected ? "white" : "#424242"
        font.pixelSize: 14
    }
    
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        
        onClicked: {
            root.sortRequested(root.sortType)
            root.isSelected = true
            root.clicked()
        }
    }
}