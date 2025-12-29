import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Button {
    id: root
    property var fileData
    property var adapter
    text: "下载"
    
    onClicked: downloadDialog.open()
    
    // 下载对话框
    Dialog {
        id: downloadDialog
        property var fileData: root.fileData
        property var adapter: root.adapter
        
        modal: true
        title: "下载选项"
        standardButtons: Dialog.Ok | Dialog.Cancel
        
        ColumnLayout {
            width: parent ? parent.width : 400
            spacing: 10
            
            RadioButton {
                id: currentNotebookRadio
                text: "到当前笔记本"
                checked: true
            }
            
            RadioButton {
                id: specificNotebookRadio
                text: "到指定笔记本"
                onCheckedChanged: if (checked && notebookCombo.model.length > 0) notebookCombo.popup.open()
                
                ComboBox {
                    id: notebookCombo
                    model: ["笔记本A", "笔记本B", "笔记本C"]
                    Layout.fillWidth: true
                    enabled: specificNotebookRadio.checked
                }
            }
            
            RadioButton {
                id: localComputerRadio
                text: "到此电脑"
                onCheckedChanged: {
                    if (checked && adapter) {
                        var path = adapter.explore("选择保存路径")
                        if (path) {
                            // 保存路径到某个变量，这里需要一个属性来存储
                            localPath = path
                        }
                    }
                }
                
                // 添加一个文本显示选择的路径
                Label {
                    id: pathLabel
                    text: localPath ? "路径: " + localPath : "未选择路径"
                    visible: localComputerRadio.checked
                    Layout.fillWidth: true
                    elide: Text.ElideMiddle
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
                    // 下载到当前笔记本
                    notebookName = "" // 空表示当前笔记本
                } else if (specificNotebookRadio.checked) {
                    // 下载到指定笔记本
                    notebookName = notebookCombo.currentText
                } else if (localComputerRadio.checked) {
                    // 下载到此电脑
                    targetPath = localPath
                }
                
                // 调用适配器的下载方法
                adapter.downloadFile(fileData.cloudId, targetPath, notebookName, mappingCheckbox.checked)
            }
        }
    }
    
    // 存储本地路径的属性
    property string localPath: ""
}