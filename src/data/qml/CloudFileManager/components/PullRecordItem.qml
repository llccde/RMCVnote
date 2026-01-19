import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts
Rectangle {
    id: root
    property var recordData
    
    height: 50
    color: "white"
    border.color: "#eee"
    
    RowLayout {
        anchors.fill: parent
        anchors.margins: 8
        
        Text {
            text: recordData.time || ""
            Layout.fillWidth: true
        }
        
        Text {
            text: "设备: " + (recordData.deviceRemark || "未知设备")
            color: "#666"
            font.pixelSize: 12
        }
    }
}
