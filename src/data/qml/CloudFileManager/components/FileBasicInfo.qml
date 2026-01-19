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
        anchors.topMargin:5
        anchors.fill: parent
        clip: true
        
        ScrollBar.vertical.policy: ScrollBar.AsNeeded
        
        ColumnLayout {
            width: parent.width
            spacing: 8

            
            Text {
                text: "文件基础信息"
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
                    text: "文件名称:"
                    color: "#6c757d"
                    font.pixelSize: 12
                }
                
                Text {
                    text: root.fileInfo ? root.fileInfo.fileName : ""
                    font.pixelSize: 12
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                }
                
                Text {
                    text: "云端ID:"
                    color: "#6c757d"
                    font.pixelSize: 12
                }
                
                Text {
                    text: root.fileInfo ? root.fileInfo.cloudId : ""
                    font.pixelSize: 12
                    font.family: "monospace"
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                }
                
                Text {
                    text: "描述:"
                    color: "#6c757d"
                    font.pixelSize: 12
                    Layout.alignment: Qt.AlignTop
                }
                
                Text {
                    text: root.fileInfo ? root.fileInfo.description : "无描述"
                    font.pixelSize: 12
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    Layout.maximumWidth: parent.width / 2 - 20
                }
                
                Text {
                    text: "笔记本:"
                    color: "#6c757d"
                    font.pixelSize: 12
                }
                
                Text {
                    text: root.fileInfo ? root.fileInfo.notebookName : "未指定"
                    font.pixelSize: 12
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                }
                
                Text {
                    text: "最后更新:"
                    color: "#6c757d"
                    font.pixelSize: 12
                }
                
                Text {
                    text: root.fileInfo ? root.fileInfo.lastUpdate : "未知"
                    font.pixelSize: 12
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                }
            }
        }
    }
}
