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
    property bool historyListVisible : true;
    property bool expanded: false
    property var noteDetails: ({})
    property var versionList: []
    property string cloudFileName: ""

    signal syncClicked()
    signal viewVersionClicked(int version)
    signal restoreVersionClicked(int version)
    function updateNoteDetails() {
        if (adapter && noteId !== 0 && notebookId !== -1) {
            var details = adapter.getNoteDetails(noteId, notebookId);
            if (details) {
                // 在detail内部先把数据处理完
                var processedDetails = details;
                var processedVersionList = processedDetails.versions;

                // 为versionList中的每个对象添加连续的showID字段
                if (processedVersionList && processedVersionList.length > 0) {
                    // 先按versionID排序（如果需要的话）
                    processedVersionList.sort((a, b) => b.version - a.version);

                    // 为每个对象添加showID，从1开始连续递增
                    for (let i = 0; i < processedVersionList.length; i++) {
                        processedVersionList[i].showVerID = processedVersionList.length-i;
                    }
                }

                // 处理完成后统一赋值给root
                root.noteDetails = processedDetails;
                root.versionList = processedVersionList;

                if (processedDetails.syncStatus == 0 || processedDetails.syncStatus == 1 || processedDetails.syncStatus == 3) {
                    historyListVisible = true;
                } else {
                    historyListVisible = false;
                }
            }
        }
    }
    width: parent ? parent.width : 100
    height: expanded ? 220 : 30
    radius: 4
    color: mouseArea.containsMouse ? "#e8e8e8" : "#ffffff"
    border.color: mouseArea.containsPress ? "#3498db" : "transparent"
    border.width: 1
    Behavior on color {
        ColorAnimation { duration: 200 } // 200毫秒的过渡时间
    }
    Component.onCompleted: {
        updateNoteDetails();
        // Get cloud filename when component is loaded
        if (adapter && noteId !== 0 && notebookId !== -1) {
            adapter.getCloudFileName(notebookId, noteId);
        }
    }
    Connections {
    target: adapter
    enabled: adapter !== null
    onNoteDetailsChanged: {
        if (changedNoteId === root.noteId && changedNotebookId === root.notebookId) {
            updateNoteDetails();
            // Also get the cloud filename when note details change
            if (adapter) {
                adapter.getCloudFileName(root.notebookId, root.noteId);
            }
        }
    }
    onCloudFileReturn: {
        if (notebookId === root.notebookId && noteId === root.noteId) {
            root.cloudFileName = cloudName;
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
                    case 0: return "不是最新";
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
            
            // // 如果展开，则获取详细信息
            // if (root.expanded && adapter && noteId !== 0 && notebookId !== -1) {
            //     var details = adapter.getNoteDetails(noteId, notebookId);
            //     if (details) {
            //         root.noteDetails = details;
                    

            //     }
            // }
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
            id: noteNameLabel
            
            text: root.noteName
            font.pixelSize: 12
            font.bold: true
            color: noteNameMouseArea.containsMouse ? "#3498db" : "#333"
            width: implicitWidth
            elide: Text.ElideRight
            anchors.verticalCenter: parent.verticalCenter
            MouseArea {
                id: noteNameMouseArea
                anchors.fill: noteNameLabel
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    if (root.adapter) {
                        root.adapter.openNoteInViewArea(root.noteId, root.notebookId);
                    }
                }
            }
        }

        // ID标签，根据冲突情况改变颜色
        Item {
            id: idContainer
            width: idLabel.implicitWidth + (cloudFileNameLabel.visible ? cloudFileNameLabel.implicitWidth + 10 : 0)
            height: Math.max(idLabel.height, cloudFileNameLabel.height)
            anchors.verticalCenter:parent.verticalCenter

            Label {
                id: idLabel
                text: "ID: " + noteId
                font.pixelSize: 10
                color: "#7f8c8d"
                anchors.verticalCenter: parent.verticalCenter
            }

            // 云端文件名标签，仅在有同步关系时显示
            Label {
                id: cloudFileNameLabel
                anchors.leftMargin:10
                text: "cloudID:" + root.cloudFileName
                font.pixelSize: 10
                color: (root.noteDetails.conflictWith && root.noteDetails.conflictWith.length > 0) ? "red" : "#7f8c8d"
                anchors.left: idLabel.right
                anchors.verticalCenter: parent.verticalCenter
                visible: root.cloudFileName && root.cloudFileName.length > 0
            }

            // 鼠标悬停区域，用于显示冲突信息
            MouseArea {
                id: idMouseArea
                anchors.fill: parent
                hoverEnabled: true

                ToolTip {
                    id: idTooltip
                    visible: idMouseArea.containsMouse && root.noteDetails.conflictWith && root.noteDetails.conflictWith.length > 0
                    delay: 300
                    text: {
                        if (root.noteDetails.conflictWith && root.noteDetails.conflictWith.length > 0) {
                            var tooltipText = "存在冲突的笔记:\n";
                            for (var i = 0; i < root.noteDetails.conflictWith.length; i++) {
                                tooltipText += (i + 1) + ". " + root.noteDetails.conflictWith[i] + "\n";
                            }
                            return tooltipText;
                        }
                        return "";
                    }
                }
            }
        }
        Rectangle{
            visible:true
            height:30
            width:30
            color:"transparent"
        }
    }
    Button {
        id: syncBtn
        text: {
            switch(root.syncStatus) {
            case 0: return "上传保存";
            case 1: return "上传保存";
            case 2: return "创建同步";
            case 3: return "未知";
            default: return "未知";
            }
        }
        flat: true
        font.pixelSize: 10
        enabled:true
        onClicked: {
            if (root.adapter && root.noteId !== 0 && root.notebookId !== -1) {
                switch(root.syncStatus){
                    case 0:case 1:
                        root.adapter.saveToCloud(root.notebookId,root.noteId);
                        break;
                    case 2:
                        root.adapter.syncNote(root.noteId, root.notebookId);
                        break;
                }

            }
        }
        anchors.top:topRow.top
        anchors.right:topRow.right
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
                id: snapshotContainer
                visible: root.historyListVisible
                width: parent.width
                height: 30
                color: "transparent"
                z:100
                // 状态变量
                property bool creatingSnapshot: false
                property string snapshotDescription: ""
                
                Column {
                    width: parent.width
                    
                    // 第一行：云端ID和快照按钮
                    Row {
                        id: headerRow
                        width: parent.width
                        height: 30
                        spacing: 8
                        
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
                        
                        // 创建/提交快照按钮
                        Button {
                            id: createSnapshotBtn
                            text: snapshotContainer.creatingSnapshot ? "确定" : "创建新版本"
                            height: 24
                            width: 80
                            anchors.verticalCenter: parent.verticalCenter
                            flat: true  // 默认无边框

                            // 添加悬停效果
                            background: Rectangle {
                                color: "transparent"
                                border.color: createSnapshotBtn.hovered ? '#364c36' : "transparent"
                                border.width: createSnapshotBtn.hovered ? 1 : 0
                                radius: 2
                            }

                            onClicked: {
                                if (snapshotContainer.creatingSnapshot) {
                                    // 提交快照
                                    adapter.snapshot(notebookId,noteId,snapshotContainer.snapshotDescription)
                                    // 重置状态
                                    snapshotContainer.creatingSnapshot = false
                                    snapshotContainer.snapshotDescription = ""
                                    snapshotDescriptionInput.text = ""
                                } else {
                                    // 开始创建快照
                                    snapshotContainer.creatingSnapshot = true
                                }
                            }
                        }

                        // 取消创建按钮（仅创建快照时显示）
                        Button {
                            id: cancelSnapshotBtn
                            text: "取消创建"
                            height: 24
                            width: 80
                            anchors.verticalCenter: parent.verticalCenter
                            visible: snapshotContainer.creatingSnapshot
                            flat: true  // 默认无边框

                            // 添加悬停效果
                            background: Rectangle {
                                color: "transparent"
                                border.color: cancelSnapshotBtn.hovered ? '#583b3b' : "transparent"
                                border.width: cancelSnapshotBtn.hovered ? 1 : 0
                                radius: 2
                            }

                            onClicked: {
                                // 取消创建
                                snapshotContainer.creatingSnapshot = false
                                snapshotContainer.snapshotDescription = ""
                                snapshotDescriptionInput.text = ""
                            }
                        }

                    }
                    
                    // 第二行：快照描述输入框（仅创建快照时显示）
                    Rectangle {
                        z:100
                        id: snapshotInputRow
                        width: parent.width
                        height: snapshotContainer.creatingSnapshot ? 30 : 0
                        color: "transparent"
                        clip: true
                        
                        Behavior on height {
                            NumberAnimation { duration: 200 }
                        }
                        
                        Row {
                            spacing: 8
                            anchors.fill: parent
                            visible: snapshotContainer.creatingSnapshot
                            
                            TextField {
                                id: snapshotDescriptionInput
                                width: parent.width - 50
                                height: 24
                                anchors.verticalCenter: parent.verticalCenter
                                placeholderText: "请输入快照描述..."
                                font.pixelSize: 11
                                
                                onTextChanged: {
                                    snapshotContainer.snapshotDescription = text
                                }
                                
                                onAccepted: {
                                    if (text.trim() !== "") {
                                        adapter.snapshot(notebookId,noteId,text)
                                        snapshotContainer.creatingSnapshot = false
                                        snapshotContainer.snapshotDescription = ""
                                        text = ""
                                    }
                                }
                            }
                        }
                    }
                }
                Button {
                    id: removeSyncBtn
                    text: "取消同步"
                    flat: true
                    font.pixelSize: 10
                    visible: root.syncStatus !== 2  // 只有在已同步状态下才显示取消同步按钮
                    onClicked: {
                        if (root.adapter && root.noteId !== 0 && root.notebookId !== -1) {
                            root.adapter.removeSync(root.notebookId, root.noteId);
                        }
                    }
                    anchors.verticalCenter: headerRow.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 0
                }
            }

            // 版本列表标题
            Rectangle {
                width: parent.width
                height: 20
                color: "transparent"
                visible:root.historyListVisible
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
                interactive: versionList.length > 3  
                
                // 添加滚动条
                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded  // 需要时显示
                }
                
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
                            text: "版本 " + modelData.showVerID+(modelData.isCurrent?"(当前)":"")
                            font.pixelSize: 10
                            color: {
                                if(!modelData.isCurrent){
                                    return "#444"
                                }else{
                                    return '#17609b'
                                }

                                }
                            anchors.verticalCenter: parent.verticalCenter
                            width: 60
                        }
                        
                        Label {
                            text: modelData.time
                            font.pixelSize: 10
                            color: "#777"
                            anchors.verticalCenter: parent.verticalCenter
                            width: 100
                        }
                        
                        Label {
                            text: modelData.size
                            font.pixelSize: 10
                            color: "#777"
                            anchors.verticalCenter: parent.verticalCenter
                            width: 60
                        }
                        Label {
                            text: modelData.changeDescription
                            font.pixelSize: 10
                            color: "#777"
                            anchors.verticalCenter: parent.verticalCenter
                            width: 60
                        }
                        Button {
                            text: "查看"
                            flat: true
                            width:50
                            font.pixelSize: 9
                            onClicked: {
                                viewVersionClicked(modelData.version);
                                if (adapter) {
                                    adapter.openNoteVersion(noteId, notebookId, modelData.version);
                                }
                            }
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        
                        Button {
                            text: "恢复"
                            flat: true
                            width:50
                            font.pixelSize: 9
                            onClicked: {
                                restoreVersionClicked(modelData.version);
                                if (adapter) {
                                    adapter.restoreNoteVersion(noteId, notebookId, modelData.version);
                                }
                            }
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        Button {
                            text: "删除"
                            flat: true
                            width:50
                            font.pixelSize: 9
                            visible: !modelData.isCurrent  // 只有非当前版本才显示删除按钮
                            onClicked: {
                                if (adapter) {
                                    adapter.deleteVersion(noteId, notebookId, modelData.version);
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
