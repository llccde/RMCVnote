import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import VNoteX 1.0
import "components"

Page {
    id: root
    property CloudFileManagerAdapter managerAdapter : CloudFileManagerAdapter
    
    // 使用Column布局确保严格上下排列
    Column {
        anchors.fill: parent
        spacing: 0
        
        // 顶部状态栏 - 固定高度
        HeaderToolbar {
            id: headerToolbar
            width: parent.width
            height: 30
            adapter: root.managerAdapter
            z: 100
        }
        
        // 主内容区域 - 占据剩余空间
        SplitView {
            id: mainSplitView
            width: parent.width
            height: parent.height - headerToolbar.height
            orientation: Qt.Horizontal
            
            // 左侧文件列表区域
            FileListPanel {
                id: leftPanel
                SplitView.preferredWidth: parent.width
                SplitView.minimumWidth: 300
                adapter: root.managerAdapter
                onFileSelected: {
                    rightPanel.fileInfo = fileData
                    rightPanel.visible = true
                    leftPanel.splitViewWidth = 0.2 * root.width
                }
            }
            
            // 右侧文件详情区域
            FileDetailPanel {
                id: rightPanel
                visible: false
                SplitView.preferredWidth: root.width * 0.8
                SplitView.minimumWidth: 400
                adapter: root.managerAdapter
                onCloseRequested: {
                    rightPanel.visible = false
                    leftPanel.splitViewWidth = root.width
                }
            }
        }
    }
}