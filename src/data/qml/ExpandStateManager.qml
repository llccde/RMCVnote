// ExpandStateManager.qml
// 仅内存的展开状态管理器
pragma Singleton
import QtQuick 2.15

QtObject {
    id: stateManager
    
    // 存储笔记本展开状态的Map
    property var expandedStates: ({})
    
    // 获取笔记本的展开状态
    function getExpandedState(notebookId) {
        return expandedStates[notebookId] || false
    }
    
    // 设置笔记本的展开状态
    function setExpandedState(notebookId, expanded) {
        expandedStates[notebookId] = expanded
    }
    
    // 清除所有展开状态
    function clearExpandedStates() {
        expandedStates = {}
    }
    
    // 获取所有展开的笔记本ID
    function getExpandedNotebookIds() {
        var ids = []
        for (var id in expandedStates) {
            if (expandedStates[id]) {
                ids.push(id)
            }
        }
        return ids
    }
}