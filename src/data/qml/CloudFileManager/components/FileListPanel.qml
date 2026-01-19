import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    property var adapter
    property real splitViewWidth: parent.width
    signal fileSelected(var fileData)
    
    // 添加一个属性来存储当前排序方式
    property int currentSortBy: 0
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        
        // 排序工具栏
        SortToolbar {
            id: sortToolbar
            Layout.fillWidth: true
            Layout.preferredHeight: 25
            onSortChanged: {
                root.currentSortBy = sortBy
                loadFileList(sortBy)
            }
        }
        
        // 文件列表
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ListView {
                id: fileListView
                model: []  // 初始化为空数组
                spacing: 2
                
                delegate: FileItem {
                    width: fileListView.width
                    fileData: modelData  // 注意：使用modelData而不是model
                    adapter: root.adapter
                    onClicked: root.fileSelected(modelData)
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
    
    // 加载文件列表的函数
    function loadFileList(sortBy) {
        var fileList = adapter.getFileList(sortBy)
        if (fileList) {
            fileListView.model = []
            fileListView.model = fileList
        } else {
            fileListView.model = []
        }
    }
    
    // 刷新文件列表
    function refresh() {
        loadFileList(currentSortBy)
    }
    
    Component.onCompleted: {
        if (adapter) {
            loadFileList(0) // 默认按名称排序
        }
    }
    
    // 当adapter改变时重新加载
    onAdapterChanged: {
        if (adapter) {
            loadFileList(currentSortBy)
        }
    }
}