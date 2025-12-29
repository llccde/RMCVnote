import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    property var adapter
    
    width: 200
    height: accountButton.height
    
    Button {
        id: accountButton
        text: adapter && adapter.isLoggedIn ? 
              (adapter.accountName || "已登录") : "未登录"
        onClicked: accountPopup.open()
    }
    
    Popup {
        id: accountPopup
        y: accountButton.height
        width: 300
        height: contentColumn.height + 20
        
        ColumnLayout {
            id: contentColumn
            width: parent.width
            spacing: 10
            
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
            spacing: 10
            
            Label {
                text: "账号: " + (root.adapter.accountName || "")
                Layout.fillWidth: true
            }
            
            TextField {
                id: deviceRemarkField
                placeholderText: "备注此设备"
                text: root.adapter.deviceRemark || ""
                Layout.fillWidth: true
                onEditingFinished: {
                    if (root.adapter) {
                        root.adapter.deviceRemark = text
                    }
                }
            }
            
            Button {
                text: "退出登录"
                Layout.fillWidth: true
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
            spacing: 10
            
            TextField {
                id: usernameField
                placeholderText: "账号"
                Layout.fillWidth: true
            }
            
            TextField {
                id: passwordField
                placeholderText: "密码"
                echoMode: TextInput.Password
                Layout.fillWidth: true
            }
            
            CheckBox {
                id: rememberCheck
                text: "记住此账户"
            }
            
            Button {
                text: "登录"
                Layout.fillWidth: true
                onClicked: {
                    if (root.adapter) {
                        root.adapter.login(usernameField.text, passwordField.text, rememberCheck.checked)
                    }
                    accountPopup.close()
                }
            }
        }
    }
}