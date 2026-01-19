import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Popup {
    id: root
    property var fileInfo
    property var adapter
    
    modal: true
    width: 800
    height: 600
    padding: 0
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        
        // 标题栏
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: '#565656'
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                
                Text {
                    text: "文件历史版本 - " + (root.fileInfo ? root.fileInfo.fileName : "")
                    color: "white"
                    font.bold: true
                    font.pixelSize: 16
                }
                
                Item { Layout.fillWidth: true }
                
                Button {
                    z:1000
                    text: "关闭"
                    onClicked: root.close()
                    
                    background: Rectangle {
                        color: "transparent"
                        border.color: "white"
                        radius: 4
                    }
                    
                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }
        
        // 版本列表
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                id: historyListView
                model: ListModel {
                    id: historyModel
                }
                spacing: 1
                
                delegate: Rectangle {
                    width: historyListView.width
                    height: 70
                    color: index % 2 === 0 ? "#ffffff" : "#f8f9fa"
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 15
                        
                        // 版本序号
                        Rectangle {
                            width: 30
                            height: 30
                            radius: 15
                            color: "#2196F3"
                            
                            Text {
                                anchors.centerIn: parent
                                text: model.versionNumber || index + 1
                                color: "white"
                                font.bold: true
                            }
                        }
                        
                        // 版本信息
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 5
                            
                            RowLayout {
                                Text {
                                    text: model.versionName || "版本 " + (index + 1)
                                    font.bold: true
                                    font.pixelSize: 14
                                }
                                
                                Item { Layout.fillWidth: true }
                                
                                Text {
                                    text: model.createTime || ""
                                    color: "#666"
                                    font.pixelSize: 12
                                }
                            }
                            
                            Row {
                                spacing: 20
                                
                                Text {
                                    text: "大小: " + (model.size || "0") + " KB"
                                    color: "#666"
                                    font.pixelSize: 12
                                }
                                
                                Text {
                                    text: "与上一个版本差异: " + (model.difference || "0%")
                                    color: "#666"
                                    font.pixelSize: 12
                                }
                            }
                        }
                        
                        // 下载按钮
                        Button {
                            text: "下载"
                            onClicked: {
                                if (root.adapter && root.fileInfo && model.versionId) {
                                    var path = root.adapter.explore("选择保存位置")
                                    if (path) {
                                        root.adapter.downloadVersion(root.fileInfo.cloudId, 
                                                                      model.versionId, 
                                                                      path)
                                    }
                                }
                            }
                            
                            background: Rectangle {
                                color: "#4CAF50"
                                radius: 4
                            }
                            
                            contentItem: Text {
                                text: parent.text
                                color: "white"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }
                    
                    // 分割线
                    Rectangle {
                        anchors.bottom: parent.bottom
                        width: parent.width
                        height: 1
                        color: "#e0e0e0"
                    }
                }
            }
        }
    }
    
    onOpened: {
        if (adapter && fileInfo) {
            var history = adapter.getFileHistory(fileInfo.cloudId)
            historyListView.model.clear()
            if (true) {
                history.forEach(function(item) {
                    historyListView.model.append(item)
                })
            }
        }
    }
}
