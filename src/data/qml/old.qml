import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    color: "#f5f5f5"
    
    // 属性：当前同步状态
    property string syncStatus: cloudSyncPanel.syncStatus
    
    // 属性：笔记本列表
    property var notebookList: []
    
    // 笔记本项组件
    Component {
        id: notebookItemComponent
        
        Rectangle {
            id: notebookItem
            required property var modelData
            property bool expanded: false
            property var noteList: []
            
            width: notebookColumn.width
            height: contentColumn.height + 20
            radius: 8
            color: index % 2 === 0 ? "#ffffff" : "#fafafa"
            border.color: "#e0e0e0"
            border.width: 1
            
            // 确保边框正确显示
            clip: false
            
            // 内容列
            Column {
                id: contentColumn
                width: parent.width - 20
                anchors.centerIn: parent
                spacing: 5
                
                // 笔记本标题栏
                Row {
                    width: parent.width
                    spacing: 10
                    
                    Image {
                        source: "qrc:/icons/notebook.svg"
                        sourceSize: Qt.size(16, 16)
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    
                    Label {
                        text: notebookItem.modelData.name
                        font.bold: true
                        font.pixelSize: 14
                        color: "#333"
                        width: parent.width - 250
                        elide: Text.ElideRight
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    
                    Label {
                        text: "ID: " + notebookItem.modelData.id
                        font.pixelSize: 11
                        color: "#999"
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    
                    Button {
                        text: notebookItem.expanded ? "收起" : "展开"
                        flat: true
                        onClicked: {
                            notebookItem.expanded = !notebookItem.expanded
                            if (notebookItem.expanded && notebookItem.noteList.length === 0) {
                                // 加载笔记列表
                                var notes = cloudSyncPanel.getNotes(notebookItem.modelData.id)
                                notebookItem.noteList = notes
                            }
                        }
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    
                    Button {
                        text: "同步"
                        flat: true
                        onClicked: {
                            cloudSyncPanel.syncNotebook(notebookItem.modelData.id)
                        }
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
                
                // 笔记列表（可展开）
                Column {
                    id: notesContainer
                    width: parent.width
                    spacing: 5
                    visible: notebookItem.expanded
                    
                    // 笔记列表标题
                    Row {
                        width: parent.width
                        visible: notebookItem.noteList.length > 0
                        
                        Rectangle {
                            width: parent.width
                            height: 1
                            color: "#eee"
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        
                        Label {
                            text: "笔记列表 (" + notebookItem.noteList.length + ")"
                            font.pixelSize: 12
                            color: "#666"
                            anchors.verticalCenter: parent.verticalCenter
                            padding: 5
                        }
                        
                        Rectangle {
                            width: parent.width
                            height: 1
                            color: "#eee"
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }
                    
                    // 笔记列表
                    Repeater {
                        model: notebookItem.noteList
                        
                        Rectangle {
                            width: parent.width
                            height: 30
                            radius: 4
                            color: index % 2 === 0 ? "#f9f9f9" : "#ffffff"
                            
                            Row {
                                anchors.fill: parent
                                anchors.leftMargin: 30
                                anchors.rightMargin: 10
                                spacing: 10
                                
                                Image {
                                    source: "qrc:/icons/note.svg"
                                    sourceSize: Qt.size(12, 12)
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                                
                                Label {
                                    text: modelData.name
                                    font.pixelSize: 12
                                    color: "#444"
                                    width: parent.width - 200
                                    elide: Text.ElideRight
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                                
                                Label {
                                    text: "ID: " + modelData.id
                                    font.pixelSize: 10
                                    color: "#aaa"
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                                
                                Button {
                                    text: "同步"
                                    flat: true
                                    font.pixelSize: 10
                                    onClicked: {
                                        cloudSyncPanel.syncNote(modelData.id, notebookItem.modelData.id)
                                    }
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                                
                                Button {
                                    text: "查看"
                                    flat: true
                                    font.pixelSize: 10
                                    onClicked: {
                                        cloudSyncPanel.openNote(modelData.id, notebookItem.modelData.id)
                                    }
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                            }
                        }
                    }
                    
                    // 空状态
                    Label {
                        width: parent.width
                        text: "暂无笔记"
                        font.pixelSize: 12
                        color: "#999"
                        horizontalAlignment: Text.AlignHCenter
                        padding: 10
                        visible: notebookItem.noteList.length === 0 && notebookItem.expanded
                    }
                }
            }
            
            // 展开/收起动画（可选）
            Behavior on height {
                NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
            }
        }
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 15
        
        // 标题栏
        RowLayout {
            Layout.fillWidth: true
            
            Label {
                text: "云同步管理"
                font.bold: true
                font.pixelSize: 18
                color: "#333"
            }
            
            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: "#ddd"
            }
            
            // 同步状态指示器
            Rectangle {
                id: statusIndicator
                width: 12
                height: 12
                radius: 6
                color: {
                    switch(syncStatus) {
                    case "idle": return "gray"
                    case "syncing": return "orange"
                    case "success": return "green"
                    case "error": return "red"
                    default: return "gray"
                    }
                }
            }
            
            Label {
                id: statusLabel
                text: {
                    switch(syncStatus) {
                    case "idle": return "空闲"
                    case "syncing": return "同步中..."
                    case "success": return "同步成功"
                    case "error": return "同步失败"
                    default: return "未知"
                    }
                }
                color: "#666"
                font.pixelSize: 12
            }
            
            Button {
                text: "刷新"
                onClicked: {
                    cloudSyncPanel.refreshNotebooks()
                }
            }
            
            Button {
                text: "全部同步"
                onClicked: {
                    cloudSyncPanel.syncAllNotebooks()
                }
            }
        }
        
        // 笔记本列表区域 - 使用ScrollView包装
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            
            // 内容区域
            Column {
                id: notebookColumn
                width: parent.width
                spacing: 10
                
                Repeater {
                    id: notebookRepeater
                    model: notebookList
                    delegate: notebookItemComponent
                }
                
                // 空状态
                Rectangle {
                    width: parent.width
                    height: 100
                    color: "transparent"
                    visible: notebookList.length === 0
                    
                    Column {
                        anchors.centerIn: parent
                        spacing: 10
                        
                        Image {
                            source: "qrc:/icons/empty.svg"
                            sourceSize: Qt.size(48, 48)
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        
                        Label {
                            text: "暂无笔记本"
                            font.pixelSize: 14
                            color: "#999"
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        
                        Button {
                            text: "添加笔记本"
                            anchors.horizontalCenter: parent.horizontalCenter
                            onClicked: {
                                cloudSyncPanel.addNotebook()
                            }
                        }
                    }
                }
            }
        }
        
        // 底部状态栏
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            Label {
                text: "笔记本数量: " + notebookList.length
                font.pixelSize: 12
                color: "#666"
            }
            
            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: "transparent"
            }
            
            Label {
                id: lastSyncLabel
                text: "上次同步: --"
                font.pixelSize: 12
                color: "#999"
            }
        }
    }
    
    // 初始化加载笔记本列表
    Component.onCompleted: {
        console.log("CloudSyncPanel QML loaded")
        cloudSyncPanel.refreshNotebooks()
    }
    
    // 监听同步状态变化
    Connections {
        target: cloudSyncPanel
        function onSyncStatusChanged(status) {
            console.log("Sync status changed to:", status)
        }
    }
    
    // 监听笔记本列表变化
    Connections {
        target: cloudSyncPanel
        function onNotebookListChanged(notebooks) {
            console.log("Notebook list updated, count:", notebooks.length)
            notebookList = []
            notebookList = notebooks
            
            // 更新最后同步时间
            var now = new Date()
            lastSyncLabel.text = "上次同步: " + now.toLocaleTimeString(Qt.locale(), "HH:mm:ss")
        }
    }
    
    // 监听笔记列表变化
    Connections {
        target: cloudSyncPanel
        function onNoteListChanged(notebookId, notes) {
            console.log("Note list updated for notebook:", notebookId, "count:", notes.length)
            
            // 更新对应的笔记本的笔记列表
            for (var i = 0; i < notebookRepeater.count; i++) {
                var item = notebookRepeater.itemAt(i)
                if (item && item.modelData.id === notebookId) {
                    item.noteList = notes
                    break
                }
            }
            
        }
    }
    
    // UI更新函数，可由C++调用
    function updateUI() {
        console.log("Updating UI...")
        // 这里可以添加UI更新逻辑
        return true
    }
}