import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    property var adapter

    width: 180
    height: accountButton.height
    color: "transparent"

    Button {
        id: accountButton
        text: adapter && adapter.isLoggedIn ?
              (adapter.userName || "已登录") : "登录"
        flat: true
        background: Rectangle {
            color: parent.down ? "#e0e0e0" : "transparent"
            radius: 4
        }
        onClicked: accountPopup.open()
    }

    Popup {
        id: accountPopup
        y: accountButton.height + 2
        width: 200
        padding: 0

        background: Rectangle {
            color: "#ffffff"
            border.color: "#cccccc"
            border.width: 1
            radius: 4
        }

        ColumnLayout {
            width: parent.width
            spacing: 0

            // 登录状态显示
            Loader {
                id: accountLoader
                Layout.fillWidth: true
                sourceComponent: adapter && adapter.isLoggedIn ? loggedInComponent : notLoggedInComponent
            }
        }
    }

    // 已登录组件
    Component {
        id: loggedInComponent

        ColumnLayout {
            spacing: 0

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: "#f0f0f0"
            }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 50

                Column {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 4

                    Label {
                        text: root.adapter.accountName || ""
                        font.pixelSize: 14
                        color: "#333333"
                    }

                    Label {
                        text: "已登录"
                        font.pixelSize: 12
                        color: "#666666"
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: "#f0f0f0"
            }

            TextField {
                Layout.fillWidth: true
                Layout.topMargin: 8
                Layout.leftMargin: 12
                Layout.rightMargin: 12
                placeholderText: "备注此设备"
                text: root.adapter.deviceRemark || ""
                background: Rectangle {
                    color: "#fafafa"
                    border.color: "#dddddd"
                    border.width: 1
                    radius: 3
                }
                onEditingFinished: {
                    if (root.adapter && text !== root.adapter.deviceRemark) {
                        root.adapter.deviceRemark = text
                    }
                }
            }

            Button {
                Layout.fillWidth: true
                Layout.topMargin: 8
                Layout.bottomMargin: 8
                Layout.leftMargin: 12
                Layout.rightMargin: 12
                text: "退出登录"
                flat: true
                background: Rectangle {
                    color: parent.down ? "#f0f0f0" : "transparent"
                    radius: 4
                }
                contentItem: Text {
                    text: parent.text
                    color: "#666666"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    if (root.adapter) {
                        root.adapter.logout()
                    }
                    accountPopup.close()
                }
            }
        }
    }

    // 未登录组件
    Component {
        id: notLoggedInComponent

        ColumnLayout {
            spacing: 0

            TextField {
                id: usernameField
                Layout.fillWidth: true
                Layout.topMargin: 12
                Layout.leftMargin: 12
                Layout.rightMargin: 12
                placeholderText: "账号"
                background: Rectangle {
                    color: "#fafafa"
                    border.color: "#dddddd"
                    border.width: 1
                    radius: 3
                }
            }

            TextField {
                id: passwordField
                Layout.fillWidth: true
                Layout.topMargin: 8
                Layout.leftMargin: 12
                Layout.rightMargin: 12
                placeholderText: "密码"
                echoMode: TextInput.Password
                background: Rectangle {
                    color: "#fafafa"
                    border.color: "#dddddd"
                    border.width: 1
                    radius: 3
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.topMargin: 8
                Layout.leftMargin: 12
                Layout.rightMargin: 12

                CheckBox {
                    id: rememberCheck
                    text: "记住账户"
                }

                Item { Layout.fillWidth: true }

                Button {
                    text: "登录"
                    flat: true
                    background: Rectangle {
                        color: parent.down ? "#f0f0f0" : "transparent"
                        radius: 4
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "#333333"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: {
                        if (root.adapter) {
                            root.adapter.login(usernameField.text, passwordField.text, rememberCheck.checked)
                        }
                        accountPopup.close()
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 12
            }
        }
    }
}
