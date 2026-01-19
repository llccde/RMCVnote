import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    property var fileInfo
    
    color: "#f8f9fa"
    border.color: "#dee2e6"
    border.width: 1

    ScrollView {
        anchors.leftMargin: 8
        anchors.topMargin: 5
        anchors.fill: parent
        clip: true
        
        ScrollBar.vertical.policy: ScrollBar.AsNeeded
        
        ColumnLayout {
            width: parent.width
            spacing: 8
            
            Text {
                text: "上一次同步信息"
                font.bold: true
                font.pixelSize: 14
                color: "#495057"
                Layout.bottomMargin: 8
            }
            
            GridLayout {
                columns: 2
                columnSpacing: 16
                rowSpacing: 6
                Layout.fillWidth: true
                
                Text {
                    text: "来源设备:"
                    color: "#6c757d"
                    font.pixelSize: 12
                }
                
                Text {
                    text: root.fileInfo && root.fileInfo.lastSyncDevice ? 
                          root.fileInfo.lastSyncDevice.remark : "未知"
                    font.pixelSize: 12
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                }
                
                Text {
                    text: "IP地址:"
                    color: "#6c757d"
                    font.pixelSize: 12
                }
                
                Text {
                    text: root.fileInfo && root.fileInfo.lastSyncDevice ? 
                          root.fileInfo.lastSyncDevice.ip : "未知"
                    font.pixelSize: 12
                    font.family: "monospace"
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                }
                
                Text {
                    text: "操作系统:"
                    color: "#6c757d"
                    font.pixelSize: 12
                }
                
                Text {
                    text: root.fileInfo && root.fileInfo.lastSyncDevice ? 
                          root.fileInfo.lastSyncDevice.os : "未知"
                    font.pixelSize: 12
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                }
                
                Text {
                    text: "机器码:"
                    color: "#6c757d"
                    font.pixelSize: 12
                }
                
                Text {
                    text: root.fileInfo && root.fileInfo.lastSyncDevice ? 
                          root.fileInfo.lastSyncDevice.machineCode : "未知"
                    font.pixelSize: 12
                    font.family: "monospace"
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                }
                
                Text {
                    text: "同步时间:"
                    color: "#6c757d"
                    font.pixelSize: 12
                }
                
                Text {
                    text: root.fileInfo && root.fileInfo.lastSyncDevice ? 
                          root.fileInfo.lastSyncDevice.time : "未知"
                    font.pixelSize: 12
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                }
            }
        }
    }
}