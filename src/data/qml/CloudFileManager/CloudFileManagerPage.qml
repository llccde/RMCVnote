import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "components"

Page {
    id: root
    property alias adapter: managerAdapter
    
    CloudFileManagerAdapter {
        id: managerAdapter
    }
    
    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal
        
        // 左侧文件列表区域
        FileListPanel {
            id: leftPanel
            SplitView.preferredWidth: root.width
            SplitView.minimumWidth: 300
            adapter: managerAdapter
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
            adapter: managerAdapter
            onCloseRequested: {
                rightPanel.visible = false
                leftPanel.splitViewWidth = root.width
            }
        }
    }
    
    // 顶部状态栏
    HeaderToolbar {
        id: headerToolbar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 60
        adapter: managerAdapter
        z: 100
    }
}