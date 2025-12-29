import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    property var fileInfo
    property var adapter
    
    color: "#fafafa"
    border.color: "#ddd"
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        
        Label {
            text: "上传记录"
            font.bold: true
            padding: 10
            Layout.fillWidth: true
        }
        
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ListView {
                id: uploadListView
                model: ListModel {}
                spacing: 2
                
                delegate: UploadRecordItem {
                    width: uploadListView.width
                    recordData: model
                    adapter: root.adapter
                }
            }
        }
    }
    
    Component.onCompleted: {
        if (adapter && fileInfo) {
            var records = adapter.getUploadRecords(fileInfo.cloudId)
            uploadListView.model.clear()
            records.forEach(function(record) {
                uploadListView.model.append(record)
            })
        }
    }
}