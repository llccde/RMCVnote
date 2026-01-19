import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls.Material 2.12
Rectangle {
    width: 1200
    height: 800
    visible: true
    Material.theme: Material.Light
    CloudFileManagerPage {
        anchors.fill: parent
    }
}
