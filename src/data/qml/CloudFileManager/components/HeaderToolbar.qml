import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    color: "#f0f0f0"
    property var adapter
    
    RowLayout {
        anchors.fill: parent
        spacing: 20
        
        // 网络状况指示器
        NetworkStatusIndicator {
            status: adapter ? adapter.networkStatus : "未知"
            Layout.alignment: Qt.AlignVCenter
        }
        
        Item { Layout.fillWidth: true }
        
        // 账号面板
        AccountPanel {
            id: accountPanel
            adapter: root.adapter
            Layout.alignment: Qt.AlignVCenter
            Layout.rightMargin: 20
        }
    }
}