import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Button {
    id: root
    property var fileData
    property var adapter
    property string localPath: ""
    property var notebookList: []

    // 在组件加载时获取笔记本列表
    Component.onCompleted: {
        updateNotebookList();
    }

    // 监听 adapter 变化
    onAdapterChanged: {
        updateNotebookList();
    }

    // 更新笔记本列表的函数
    function updateNotebookList() {
        if (adapter) {
            root.notebookList = adapter.getNotebookList()
        }
    }
    
    text: "下载"
    flat: true
    
    background: Rectangle {
        implicitWidth: 80
        implicitHeight: 36
        radius: 6
        color: root.down ? "#E0E0E0" : root.hovered ? "#F5F5F5" : "white"
        border.color: "#E0E0E0"
        border.width: 1
    }
    
    onClicked: {
        updateNotebookList();
        downloadDialog.open();
    }
    
    Dialog {
        parent: Overlay.overlay
        anchors.centerIn: parent
        id: downloadDialog
        modal: true
        title: "下载选项"
        standardButtons: Dialog.Ok | Dialog.Cancel
        
        width: 400
        
        background: Rectangle {
            radius: 8
            color: "white"
            border.color: "#E0E0E0"
        }
        
        ColumnLayout {
            id: lay
            width: parent.width
            spacing: 12
            
            RadioButton {
                id: currentNotebookRadio
                text: "到当前笔记本"
                checked: true
            }
            
            RadioButton {
                id: specificNotebookRadio
                text: "到指定笔记本"
            }
            
            ComboBox {
                id: notebookCombo
                Layout.fillWidth: true
                Layout.preferredHeight: 40  // 设置推荐高度
                model: root.notebookList
                enabled: specificNotebookRadio.checked
                visible: specificNotebookRadio.checked
                Layout.leftMargin: 20  // 缩进显示
                Layout.rightMargin: 30  // 缩进显示
                
                // 设置内容项样式
                contentItem: Text {
                    text: notebookCombo.displayText
                    font: notebookCombo.font
                    color: notebookCombo.enabled ? "black" : "gray"
                    verticalAlignment: Text.AlignVCenter
                    leftPadding: 10
                    rightPadding: notebookCombo.indicator.width + 10
                }
                
                // 设置下拉指示器
                indicator: Rectangle {
                    x: notebookCombo.width - width
                    y: notebookCombo.topPadding + (notebookCombo.availableHeight - height) / 2
                    width: 20
                    height: 20
                    color: "transparent"
                    Text {
                        text: "▼"
                        font.pixelSize: 12
                        color: notebookCombo.enabled ? "black" : "gray"
                        anchors.centerIn: parent
                    }
                }
                
                background: Rectangle {
                    radius: 4
                    border.color: notebookCombo.enabled ? "#E0E0E0" : "#F0F0F0"
                    border.width: 1
                    color: notebookCombo.enabled ? "white" : "#F8F8F8"
                }
                
                // 设置下拉框样式
                popup: Popup {
                    y: notebookCombo.height
                    width: notebookCombo.width
                    implicitHeight: contentItem.implicitHeight
                    padding: 1
                    
                    contentItem: ListView {
                        clip: true
                        implicitHeight: contentHeight
                        model: notebookCombo.popup.visible ? notebookCombo.delegateModel : null
                        currentIndex: notebookCombo.highlightedIndex
                        
                        ScrollIndicator.vertical: ScrollIndicator { }
                    }
                    
                    background: Rectangle {
                        radius: 4
                        border.color: "#E0E0E0"
                        color: "white"
                    }
                }
                
                // 设置下拉项样式
                delegate: ItemDelegate {
                    width: notebookCombo.width
                    height: 40
                    text: modelData
                    highlighted: notebookCombo.highlightedIndex === index
                    background: Rectangle {
                        color: highlighted ? "#E0E0E0" : "transparent"
                    }
                }
            }
            
            RadioButton {
                id: localComputerRadio
                text: "到此电脑"
            }
            
            ColumnLayout {
                Layout.leftMargin: 20  // 缩进显示
                spacing: 5
                visible: localComputerRadio.checked
                
                Button {
                    text: "选择路径"
                    Layout.fillWidth: true
                    Layout.margins:30
                    Layout.preferredHeight: 30
                    onClicked: {
                        if (adapter) {
                            var path = adapter.explore("选择保存路径")
                            if (path) localPath = path
                        }
                    }
                }
                
                Label {
                    text: localPath ? "路径: " + localPath : "未选择路径"
                    Layout.fillWidth: true
                    elide: Text.ElideMiddle
                    color: localPath ? "black" : "gray"
                }
            }
            
            CheckBox {
                id: mappingCheckbox
                text: "立即建立映射关系"
                checked: true
            }
        }
        
        onAccepted: {
            if (adapter && fileData) {
                var targetPath = ""
                var notebookName = ""
                
                if (currentNotebookRadio.checked) {
                    notebookName = ""
                } else if (specificNotebookRadio.checked) {
                    notebookName = notebookCombo.currentText
                } else if (localComputerRadio.checked) {
                    targetPath = localPath
                }
                
                adapter.downloadFile(fileData.cloudId, targetPath, notebookName, mappingCheckbox.checked)
            }
        }
        
        onRejected: {
            // 重置选择
            currentNotebookRadio.checked = true
        }
    }
}
