import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    width: 1200
    height: 800
    visible: true
    title: "云端文件管理器"
    
    CloudFileManagerPage {
        anchors.fill: parent
    }
}