import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    property string status: "未知"
    
    width: 100
    height: 30
    radius: 15
    color: getStatusColor()
    
    Text {
        anchors.centerIn: parent
        text: getStatusText()
        color: "white"
        font.pixelSize: 12
    }
    
    function getStatusColor() {
        switch(status) {
            case "良好": return "#4CAF50"
            case "一般": return "#FFC107"
            case "差": return "#F44336"
            default: return "#9E9E9E"
        }
    }
    
    function getStatusText() {
        return "网络: " + status
    }
}