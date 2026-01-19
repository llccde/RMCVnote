import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    color: '#f6f6f6'
    signal sortChanged(int sortBy)
    
    // 当前排序类型，0=名称，1=笔记本，2=创建时间
    property int currentSortType: 0
    
    // 排序类型对应的文本
    readonly property var sortTexts: ["按名称排序", "按笔记本排序", "按创建时间排序"]
    
    Button {
        id: sortButton
        anchors.right:parent.right
        text: root.sortTexts[root.currentSortType]
        
        // 设置无边框样式
        flat: true
        
        background: Rectangle {
            color: sortButton.hovered ? '#d0d0d0' : 'transparent'
            radius: 4
        }
        
        // 点击按钮切换排序方式
        onClicked: {
            // 循环切换 0->1->2->0
            root.currentSortType = (root.currentSortType + 1) % 3
            // 发出排序改变信号
            root.sortChanged(root.currentSortType)
        }
    }
}