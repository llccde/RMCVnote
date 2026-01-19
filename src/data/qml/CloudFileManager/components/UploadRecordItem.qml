import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts
Rectangle {
    id: root
    property var recordData
    property var adapter
    
    height: 70
    color: "white"
    border.color: "#eee"
    
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: tooltipTimer.start()
        onExited: tooltipTimer.stop()
        
        RowLayout {
            anchors.fill: parent
            anchors.margins: 8
            
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 3
                
                RowLayout {
                    Text {
                        text: "设备: " + (recordData.deviceRemark || "未知设备")
                        font.bold: true
                    }
                    
                    Item { Layout.fillWidth: true }
                    
                    Text {
                        text: recordData.time || ""
                        color: "#666"
                        font.pixelSize: 12
                    }
                }
                
                Row {
                    spacing: 15
                    
                    Text {
                        text: "更新前: " + (recordData.beforeSize || "0") + " KB"
                        color: "#666"
                    }
                    
                    Text {
                        text: "更新后: " + (recordData.afterSize || "0") + " KB"
                        color: "#666"
                    }
                    
                    Text {
                        text: "相似度: " + (recordData.similarity || "0") + "%"
                        color: "#666"
                    }
                }
                
                // 提交信息（简短显示）
                Text {
                    text: recordData.commitMessage ? 
                          (recordData.commitMessage.length > 50 ? 
                           recordData.commitMessage.substring(0, 50) + "..." : 
                           recordData.commitMessage) : 
                          "无提交信息"
                    color: "#777"
                    font.pixelSize: 12
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
            }
        }
    }
    
    Timer {
        id: tooltipTimer
        interval: 1000
        onTriggered: {
            if (recordData.commitMessage && recordData.commitMessage.length > 50) {
                tooltip.show("提交信息: " + recordData.commitMessage)
            }
        }
    }
    
    ToolTip {
        id: tooltip
        delay: 500
        timeout: 5000
    }
}
