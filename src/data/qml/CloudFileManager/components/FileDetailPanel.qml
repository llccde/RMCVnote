import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    property var adapter
    property var fileInfo
    signal closeRequested
    
    color: "#f5f5f5"
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12  // 减少边距
        spacing: 8  // 减少间距
        
        // 顶部工具栏
        ToolBar {
            Layout.fillWidth: true
            Layout.preferredHeight: 48  // 降低高度
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 8
                
                Button {
                    flat: true
                    text: "← 返回列表"
                    onClicked: root.closeRequested()
                    
                    background: Rectangle {
                        color: parent.down ? "#e0e0e0" : "transparent"
                        radius: 2
                    }
                }
                
                Item { Layout.fillWidth: true }
                
                Button {
                    flat: true
                    text: "创建新版本"
                    onClicked: {
                        if (adapter && fileInfo) {
                            adapter.createNewVersion(fileInfo.cloudId, "新版本")
                        }
                    }
                }
                
                Button {
                    flat: true
                    text: "历史版本"
                    onClicked: {
                        historyPopup.fileInfo = root.fileInfo
                        historyPopup.adapter = root.adapter
                        historyPopup.open()
                    }
                }
            }
        }
        
        // 文件基础信息 - 移除不必要的Rectangle包装
        FileBasicInfo {
            Layout.fillWidth: true
            Layout.preferredHeight: 140  // 稍微降低高度
            fileInfo: root.fileInfo
        }
        
        // 上一次同步设备信息 - 移除不必要的Rectangle包装
        LastSyncInfo {
            Layout.fillWidth: true
            Layout.preferredHeight: 140  // 降低高度
            fileInfo: root.fileInfo
        }
        
        // 上传和拉取记录区域 - 简化布局
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8
            
            // 上传记录
            UploadRecordList {
                Layout.fillWidth: true
                Layout.fillHeight: true
                fileInfo: root.fileInfo
                adapter: root.adapter
            }
            
            // 拉取记录
            PullRecordList {
                Layout.fillWidth: true
                Layout.fillHeight: true
                fileInfo: root.fileInfo
                adapter: root.adapter
            }
        }
    }
    
    FileHistoryPanel {
        id: historyPopup
    }
}
