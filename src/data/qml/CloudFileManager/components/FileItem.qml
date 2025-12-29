import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    property var fileData
    property var adapter
    signal clicked
    
    height: 80
    color: mouseArea.containsMouse ? "#f5f5f5" : "white"
    
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
        
        RowLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 15
            
            // 文件图标
            Rectangle {
                width: 50
                height: 50
                radius: 5
                color: "#e3f2fd"
                Layout.alignment: Qt.AlignVCenter
                
                Text {
                    anchors.centerIn: parent
                    text: "📄"
                    font.pixelSize: 24
                }
            }
            
            // 文件信息
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 5
                
                Text {
                    text: fileData.fileName || "未命名"
                    font.bold: true
                    font.pixelSize: 16
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
                
                Row {
                    spacing: 20
                    
                    Text {
                        text: "笔记本: " + (fileData.notebookName || "未指定")
                        color: "#666"
                        font.pixelSize: 12
                    }
                    
                    Text {
                        text: "更新: " + (fileData.lastUpdate || "未知")
                        color: "#666"
                        font.pixelSize: 12
                    }
                    
                    Text {
                        text: "云端ID: " + (fileData.cloudId || "")
                        color: "#999"
                        font.pixelSize: 10
                    }
                }
                
                Text {
                    text: fileData.description || "无描述"
                    color: "#777"
                    font.pixelSize: 12
                    maximumLineCount: 1
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
            }
            
            // 下载按钮
            DownloadButton {
                fileData: root.fileData
                adapter: root.adapter
                Layout.alignment: Qt.AlignVCenter
                Layout.rightMargin: 10
            }
        }
    }
}