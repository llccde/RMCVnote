import QtQuick 2.15
import QtQuick.Controls 2.15
import VNoteX 1.0

Rectangle {
    id: root

    // 属性定义
    property var note_modelData:null
    property int noteId: 0
    property string noteName: ""
    property string notePath: ""
    property int notebookId: -1
    property var adapter: null
    property int syncStatus: noteDetails.syncStatus // 0:不是最新(红), 1:是最新(绿), 2:未同步(灰), 3:后台自动同步(蓝)

    property bool expanded: false
    property var noteDetails: ({})
    property var versionList: []

    signal syncClicked()
    signal viewVersionClicked(int version)
    signal restoreVersionClicked(int version)
    function updateNoteDetails() {
        if (adapter && noteId !== 0 && notebookId !== -1) {
            var details = adapter.getNoteDetails(noteId, notebookId);
            if (details) {
                root.noteDetails = details;
            }
        }
    }
    width: parent ? parent.width : 100
    height: expanded ? 200 : 30
    radius: 4
    color: mouseArea.containsMouse ? "#e8e8e8" : "#ffffff"
    border.color: mouseArea.containsPress ? "#3498db" : "transparent"
    border.width: 1
    Behavior on color {
        ColorAnimation { duration: 200 } // 200毫秒的过渡时间
    }
    Component.onCompleted: {
        updateNoteDetails();
    }
    Connections {
    target: adapter
    enabled: adapter !== null
    onNoteDetailsChanged: {
        if (changedNoteId === root.noteId && changedNotebookId === root.notebookId) {
            updateNoteDetails();
        }
    }
}
    // 同步状态圆点
    Rectangle {
        id: statusDot
        width: 10
        height: 10
        radius: 5
        anchors {
            left: parent.left
            leftMargin: 8
            verticalCenter: topRow.verticalCenter
        }
        
        // 根据同步状态设置颜色
        color: {
            switch(root.syncStatus) {
            case 0: return "#e74c3c"; // 红 - 不是最新
            case 1: return "#2ecc71"; // 绿 - 是最新
            case 2: return "#95a5a6"; // 灰 - 未同步
            case 3: return "#3498db"; // 蓝 - 后台自动同步
            default: return "#95a5a6";
            }
        }
        
        // 鼠标悬停提示
        MouseArea {
            id: statusTooltipArea
            anchors.fill: parent
            hoverEnabled: true
            
            ToolTip {
                id: statusTooltip
                visible: statusTooltipArea.containsMouse
                delay: 300
                
                text: {
                    switch(root.syncStatus) {
                    case 0: return "不是最新版本，需要同步";
                    case 1: return "已同步，是最新版本";
                    case 2: return "未同步到云端";
                    case 3: return "后台自动同步中...";
                    default: return "未知状态";
                    }
                }
            }
        }
    }
// 鼠标交互区域
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            root.expanded = !root.expanded;
            
            // 如果展开，则获取详细信息
            if (root.expanded && adapter && noteId !== 0 && notebookId !== -1) {
                var details = adapter.getNoteDetails(noteId, notebookId);
                if (details) {
                    root.noteDetails = details;
                    
                    // 模拟版本列表（实际应从adapter获取）
                    root.versionList = [
                        {"version": 3, "time": "2023-10-15 14:30", "size": "2.1KB"},
                        {"version": 2, "time": "2023-10-14 09:15", "size": "2.0KB"},
                        {"version": 1, "time": "2023-10-13 16:45", "size": "1.9KB"}
                    ];
                }
            }
        }
    }
    Row {
        id: topRow
        width: parent.width - 20
        spacing: 8
        leftPadding: 20 // 为状态圆点留出空间

        Image {
            source: "qrc:/icons/note.svg"
            sourceSize: Qt.size(12, 12)
            anchors.verticalCenter: parent.verticalCenter
        }

        Label {
            text: noteName
            font.pixelSize: 12
            font.bold: true
            color: "#333"
            width: parent.width - 120
            elide: Text.ElideRight
            anchors.verticalCenter: parent.verticalCenter
        }

        Label {
            text: "ID: " + noteId
            font.pixelSize: 10
            color: "#7f8c8d"
            anchors.verticalCenter: parent.verticalCenter
        }

        Button {
            id: syncBtn
            text: {
                switch(root.syncStatus) {
                case 0: return "立即同步!";
                case 1: return "已同步";
                case 2: return "同步到云端";
                case 3: return "同步中...";
                default: return "同步";
                }
            }
            flat: true
            font.pixelSize: 10
            enabled: root.syncStatus !== 1 && root.syncStatus !== 3
            onClicked: {
                if (adapter && noteId !== 0 && notebookId !== -1) {
                    adapter.syncNote(noteId, notebookId);
                    syncClicked();
                }
            }
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    

    // 展开后的详细信息区域
    Rectangle {
        id: detailPanel
        visible: expanded
        anchors {
            top: topRow.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 10
            topMargin: 15
        }
        color: "transparent"

        Column {
            id: detailColumn
            width: parent.width
            spacing: 8

            // 云端ID或同步按钮
            Rectangle {
                width: parent.width
                height: 30
                color: "transparent"
                
                Row {
                    spacing: 8
                    anchors.verticalCenter: parent.verticalCenter
                    
                    Label {
                        text: "云端ID:"
                        font.pixelSize: 11
                        color: "#666"
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    
                    Loader {
                        id: cloudIdLoader
                        anchors.verticalCenter: parent.verticalCenter
                        
                        sourceComponent: root.syncStatus === 2 ? syncToCloudBtn : cloudIdLabel
                    }
                }
            }

            // 版本列表标题
            Rectangle {
                width: parent.width
                height: 20
                color: "transparent"
                
                Label {
                    text: "历史版本:"
                    font.pixelSize: 11
                    font.bold: true
                    color: "#666"
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            // 版本列表
            ListView {
                id: versionListView
                width: parent.width
                height: Math.min(versionList.length * 30, 90)
                model: versionList
                clip: true
                interactive: false
                
                delegate: Rectangle {
                    width: parent.width
                    height: 30
                    color: index % 2 === 0 ? "#f8f8f8" : "transparent"
                    radius: 3
                    
                    Row {
                        spacing: 15
                        anchors.fill: parent
                        anchors.leftMargin: 10
                        anchors.rightMargin: 10
                        
                        Label {
                            text: "版本 " + modelData.version
                            font.pixelSize: 10
                            color: "#444"
                            anchors.verticalCenter: parent.verticalCenter
                            width: 60
                        }
                        
                        Label {
                            text: modelData.time
                            font.pixelSize: 10
                            color: "#777"
                            anchors.verticalCenter: parent.verticalCenter
                            width: 150
                        }
                        
                        Label {
                            text: modelData.size
                            font.pixelSize: 10
                            color: "#777"
                            anchors.verticalCenter: parent.verticalCenter
                            width: 60
                        }
                        
                        Button {
                            text: "查看"
                            flat: true
                            font.pixelSize: 9
                            onClicked: {
                                viewVersionClicked(modelData.version);
                                if (adapter) {
                                    adapter.viewNoteVersion(noteId, notebookId, modelData.version);
                                }
                            }
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        
                        Button {
                            text: "恢复"
                            flat: true
                            font.pixelSize: 9
                            onClicked: {
                                restoreVersionClicked(modelData.version);
                                if (adapter) {
                                    adapter.restoreNoteVersion(noteId, notebookId, modelData.version);
                                }
                            }
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }
                }
            }
            
            // 更多信息
            Rectangle {
                width: parent.width
                height: 20
                color: "transparent"
                visible: noteDetails.modifiedTime
                
                Row {
                    spacing: 15
                    anchors.verticalCenter: parent.verticalCenter
                    
                    Label {
                        text: "修改时间: " + (noteDetails.modifiedTime || "")
                        font.pixelSize: 10
                        color: "#777"
                    }
                    
                    Label {
                        text: "大小: " + (noteDetails.size ? noteDetails.size + "KB" : "")
                        font.pixelSize: 10
                        color: "#777"
                    }
                    
                    Label {
                        text: "字数: " + (noteDetails.wordCount || "0")
                        font.pixelSize: 10
                        color: "#777"
                    }
                }
            }
        }
    }

    // 高度变化动画
    Behavior on height {
        NumberAnimation { duration: 250; easing.type: Easing.OutCubic }
    }



    // 子组件定义
    Component {
        id: syncToCloudBtn

        Button {
            text: "同步到云端"
            flat: true
            font.pixelSize: 10
            onClicked: {
                if (adapter && noteId !== 0 && notebookId !== -1) {
                    adapter.syncNote(noteId, notebookId);
                    syncClicked();
                }
            }
        }
    }
    
    Component {
        id: cloudIdLabel
        
        Label {
            text: noteDetails.cloudId || "N/A"
            font.pixelSize: 11
            color: "#3498db"
            font.bold: true
        }
    }
}
