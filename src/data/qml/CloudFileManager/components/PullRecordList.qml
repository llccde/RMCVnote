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
            text: "拉取记录"
            font.bold: true
            padding: 10
            Layout.fillWidth: true
        }
        
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ListView {
                id: pullListView
                model: ListModel {}
                spacing: 2
                
                delegate: PullRecordItem {
                    width: pullListView.width
                    recordData: model
                }
            }
        }
    }
    
    Component.onCompleted: {
        if (adapter && fileInfo) {
            var records = adapter.getPullRecords(fileInfo.cloudId)
            pullListView.model.clear()
            records.forEach(function(record) {
                pullListView.model.append(record)
            })
        }
    }
}