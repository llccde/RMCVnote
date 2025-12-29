import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    property var adapter
    property var fileInfo
    signal closeRequested
    
    color: "white"
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        
        // 顶部工具栏
        ToolBar {
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            
            RowLayout {
                anchors.fill: parent
                
                Button {
                    text: "← 返回列表"
                    onClicked: root.closeRequested()
                }
                
                Item { Layout.fillWidth: true }
                
                Button {
                    text: "创建新版本"
                    onClicked: {
                        if (adapter && fileInfo) {
                            adapter.createNewVersion(fileInfo.cloudId, "新版本")
                        }
                    }
                }
            }
        }
        
        // 文件基础信息
        FileBasicInfo {
            fileInfo: root.fileInfo
            Layout.fillWidth: true
            Layout.preferredHeight: 150
        }
        
        // 上一次同步设备信息
        LastSyncInfo {
            fileInfo: root.fileInfo
            Layout.fillWidth: true
            Layout.preferredHeight: 120
        }
        
        // 上传和拉取记录区域
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 10
            
            UploadRecordList {
                fileInfo: root.fileInfo
                adapter: root.adapter
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            
            PullRecordList {
                fileInfo: root.fileInfo
                adapter: root.adapter
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }
}