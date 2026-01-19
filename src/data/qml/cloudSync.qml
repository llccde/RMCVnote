import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import VNoteX 1.0

Rectangle {
    id: root
    color: "#f5f5f5"

    // 创建适配器实例
    property var adapter: CloudSyncAdapter {

        // 使用JavaScript函数形式声明信号处理器
        onNotebookListChanged: function(notebooks) {
            console.log("Notebook list updated, count:", notebooks.length)
            // 清空并重新填充模型
            notebookListModel.clear()
            for (var i = 0; i < notebooks.length; i++) {
                notebookListModel.append({
                    "id": notebooks[i].id || 0,
                    "name": notebooks[i].name || "",
                    "description": notebooks[i].description || "",
                    "type": notebooks[i].type || ""
                })
            }
            // 更新最后同步时间
            var now = new Date()
            lastSyncLabel.text = "上次同步: " + now.toLocaleTimeString(Qt.locale(), "HH:mm:ss")
        }

        onSyncStatusChanged: function(status) {
            console.log("Sync status changed to:", status)
        }

        onNoteListChanged: function(notebookId, notes) {
            console.log("Note list updated for notebook:", notebookId, "count:", notes.length)

            // 更新对应的笔记本的笔记列表
            for (var i = 0; i < notebookRepeater.count; i++) {
                var item = notebookRepeater.itemAt(i)
                if (item && item.notebookId === notebookId) {
                    item.noteList = notes
                    item.notesLoaded = true
                    break
                }
            }
        }
    }

    property string syncStatus: adapter.syncStatus

    Component.onCompleted: {
        console.log("CloudSyncPanel QML loaded")
        adapter.refreshNotebooks()
    }

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: 10
        spacing: 15

        // 标题栏 - 固定高度
        RowLayout {
            id: headerLayout
            Layout.fillWidth: true
            Layout.preferredHeight: 30

            Label {
                text: "云同步管理"
                font.bold: true
                font.pixelSize: 18
                color: "#333"
                Layout.alignment: Qt.AlignLeft
            }

            // 间隔
            Item {
                Layout.fillWidth: true
            }

            // 同步状态指示器
            RowLayout {
                spacing: 5
                Layout.alignment: Qt.AlignRight

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
                        default: return syncStatus
                        }
                    }
                    color: "#666"
                    font.pixelSize: 12
                }
            }

            Button {
                text: "刷新"
                Layout.alignment: Qt.AlignRight
                onClicked: adapter.refreshNotebooks()
            }

            Button {
                text: "全部同步"
                Layout.alignment: Qt.AlignRight
                onClicked: adapter.syncAllNotebooks()
            }
        }

        // 笔记本列表区域 - 可伸缩
        ScrollView {
            id: scrollView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            // 内容区域
            Column {
                id: notebookColumn
                width: scrollView.availableWidth  // 使用可用宽度
                spacing: 10

                Repeater {
                    id: notebookRepeater
                    model: notebookListModel

                    delegate: NotebookItem {
                        width: notebookColumn.width
                        notebookId: model.id
                        notebookName: model.name
                        notebookDescription: model.description
                        notebookType: model.type
                        adapter: root.adapter  // 直接传递适配器引用
                    }
                }

                // 空状态
                Rectangle {
                    width: parent.width
                    height: 100
                    color: "transparent"
                    visible: notebookListModel.count === 0

                    Column {
                        anchors.centerIn: parent
                        spacing: 10

                        Rectangle {
                            width: 48
                            height: 48
                            radius: 24
                            color: "#e0e0e0"
                            anchors.horizontalCenter: parent.horizontalCenter

                            Text {
                                text: "📚"
                                font.pixelSize: 24
                                anchors.centerIn: parent
                            }
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
                            onClicked: adapter.addNotebook()
                        }
                    }
                }
            }
        }

        // 底部状态栏 - 固定高度
        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 20

            Label {
                text: "笔记本数量: " + notebookListModel.count
                font.pixelSize: 12
                color: "#666"
            }

            Item {
                Layout.fillWidth: true
            }

            Label {
                id: lastSyncLabel
                text: "上次同步: --"
                font.pixelSize: 12
                color: "#999"
            }
        }
    }

    // 笔记本数据模型
    ListModel {
        id: notebookListModel
    }
}
