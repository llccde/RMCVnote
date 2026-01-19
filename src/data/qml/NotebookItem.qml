import QtQuick 2.15
import QtQuick.Controls 2.15
import VNoteX 1.0

Rectangle {
    id: notebookItem
    
    // 属性定义 - 使用具体属性而不是对象
    property int notebookId: -1
    property string notebookName: ""
    property string notebookDescription: ""
    property string notebookType: ""
    property var adapter: null  // 改为可选属性，避免绑定循环
    
    property bool expanded: false
    /**
    
    id
    name
    path
    */
    property var noteList: []
    property bool notesLoaded: false
    
    // 宽度绑定到父容器，高度自适应
    width: parent ? parent.width : 0
    height: contentColumn.implicitHeight + 20
    radius: 8
    color: index % 2 === 0 ? "#ffffff" : "#fafafa"
    border.color: "#e0e0e0"
    border.width: 1
    
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
            
            Rectangle {
                width: 16
                height: 16
                radius: 8
                color: "#4CAF50"
                anchors.verticalCenter: parent.verticalCenter
                
                Text {
                    text: "📚"
                    font.pixelSize: 10
                    anchors.centerIn: parent
                }
            }
            
            Label {
                text: notebookName
                font.bold: true
                font.pixelSize: 14
                color: "#333"
                width: parent.width - 250
                elide: Text.ElideRight
                anchors.verticalCenter: parent.verticalCenter
            }
            
            Label {
                text: "ID: " + notebookId
                font.pixelSize: 11
                color: "#999"
                anchors.verticalCenter: parent.verticalCenter
            }
            
            Item {
                width: 10
            }
            
            Button {
                text: notebookItem.expanded ? "收起" : "展开"
                flat: true
                onClicked: notebookItem.toggleExpanded()
                anchors.verticalCenter: parent.verticalCenter
            }
            
            Button {
                text: "同步"
                flat: true
                enabled: notebookId !== -1 && adapter !== null
                onClicked: function() {  // 使用函数形式
                    if (adapter && notebookId !== -1) {
                        adapter.syncNotebook(notebookId)
                    }
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
                    width: parent.width*0.4
                    height: 1
                    color: "#eee"
                    anchors.verticalCenter: parent.verticalCenter
                }
                
                Label {
                    text: "笔记列表 (" + notebookItem.noteList.length + ")"
                    font.pixelSize: 12
                    color: "#666"
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
                
                delegate: NoteItem {
                    note_modelData: modelData
                    noteId: modelData.id || 0
                    noteName: modelData.name || ""
                    notePath: modelData.path || ""
                    notebookId: notebookItem.notebookId
                    adapter: notebookItem.adapter
                    width: notesContainer.width
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
            
            // 加载中状态
            Rectangle {
                width: parent.width
                height: 30
                color: "transparent"
                visible: notebookItem.expanded && !notebookItem.notesLoaded && notebookItem.noteList.length === 0
                
                Label {
                    anchors.centerIn: parent
                    text: "加载中..."
                    font.pixelSize: 12
                    color: "#999"
                }
            }
        }
    }
    
    // 加载笔记列表
    function loadNotes() {
        var notes = notebookItem.adapter.getNotes(notebookItem.notebookId)
        console.log("notes of notebook id:"+notebookItem.notebookId+" has load,count:"+notes.length)
        if (notes && notes.length > 0) {
            notebookItem.noteList = []
            notebookItem.noteList = notes
            notebookItem.notesLoaded = true
        }
    }
    
    // 切换展开状态
    function toggleExpanded() {
        notebookItem.expanded = !notebookItem.expanded
        if (notebookItem.expanded && !notebookItem.notesLoaded) {
            notebookItem.loadNotes()
        }
    }
    
    // 展开/收起动画
    Behavior on height {
        NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
    }
}
