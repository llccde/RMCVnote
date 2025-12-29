import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    color: "#f8f8f8"
    signal sortChanged(int sortBy)
    
    Row {
        anchors.centerIn: parent
        spacing: 15
        
        SortButton {
            text: "按名称排序"
            sortType: 0
            onSortRequested: root.sortChanged(sortType)
        }
        
        SortButton {
            text: "按笔记本排序"
            sortType: 1
            onSortRequested: root.sortChanged(sortType)
        }
        
        SortButton {
            text: "按创建时间排序"
            sortType: 2
            onSortRequested: root.sortChanged(sortType)
        }
    }
}