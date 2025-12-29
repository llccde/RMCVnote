import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    property var adapter
    property real splitViewWidth: parent.width
    signal fileSelected(var fileData)
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        
        // 排序工具栏
        SortToolbar {
            id: sortToolbar
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            onSortChanged: root.adapter.getFileList(sortBy)
        }
        
        // 文件列表
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ListView {
                id: fileListView
                model: ListModel {}
                spacing: 2
                
                delegate: FileItem {
                    width: fileListView.width
                    fileData: model
                    adapter: root.adapter
                    onClicked: root.fileSelected(model)
                }
                
                onContentYChanged: {
                    // 滚动到底部加载更多
                    if (contentY + height >= contentHeight - 50) {
                        // 这里可以添加加载更多逻辑
                    }
                }
            }
        }
    }
    
    Component.onCompleted: {
        if (adapter) {
            adapter.getFileList(0) // 默认按名称排序
        }
    }
}