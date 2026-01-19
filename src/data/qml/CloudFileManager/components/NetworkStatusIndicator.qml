import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    property string status: "未知"
    
    width: 120
    height: 30
    radius: 15
    color: "#F5F5F5"  // 背景色改为浅灰色
    
    Row {
        id: contentRow
        anchors.centerIn: parent
        spacing: 8
        
        // 左边状态指示圆点
        Rectangle {
            id: statusIndicator
            width: 12
            height: 12
            radius: width / 2
            anchors.verticalCenter: parent.verticalCenter
            color: getStatusColor()
            
            // 可选：添加轻微的内阴影效果
            Rectangle {
                anchors.fill: parent
                radius: parent.radius
                color: "transparent"
                border.color: Qt.rgba(0, 0, 0, 0.1)
                border.width: 1
            }
        }
        
        // 右边状态描述文本
        Text {
            id: statusText
            text: getStatusText()
            color: "#333333"  // 深灰色文字
            font.pixelSize: 12
            font.family: "Microsoft YaHei"
            anchors.verticalCenter: parent.verticalCenter
        }
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
        // 可以根据需要调整文本格式
        switch(status) {
            case "良好": return "网络状态良好"
            case "一般": return "网络状态一般"
            case "差": return "网络状态差"
            default: return "网络状态未知"
        }
    }
}