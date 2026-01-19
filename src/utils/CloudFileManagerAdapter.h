#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantList>
#include <QDateTime>
#include <qcontainerfwd.h>
#include <qtmetamacros.h>
#include "CloudFileNetWork.h" 
#ifndef CLOUDFILEMANAGERADAPTER_H
#define CLOUDFILEMANAGERADAPTER_H

namespace vnotex{

#include <QString>
#include <QVariant>
#include <QVariantMap>
#include <QDateTime>


class CloudFileManagerAdapter : public QObject
{
    Q_OBJECT
    
    // 网络状况
    Q_PROPERTY(QString networkStatus READ networkStatus NOTIFY networkStatusChanged)
    
    // 登录状态
    Q_PROPERTY(bool isLoggedIn READ isLoggedIn NOTIFY loginStatusChanged)
    Q_PROPERTY(QString accountName READ accountName NOTIFY accountInfoChanged)
    Q_PROPERTY(QString userName READ userName NOTIFY accountInfoChanged)
    Q_PROPERTY(QString deviceRemark READ deviceRemark WRITE setDeviceRemark NOTIFY deviceRemarkChanged)
    
public:
    static CloudFileManagerAdapter* getAdapter();
    
    // 登录相关
    Q_INVOKABLE void login(const QString& username, const QString& password, bool remember);
    Q_INVOKABLE void logout();
    
    // 文件操作
    Q_INVOKABLE QVariantList getFileList(int sortBy); // 0=名称, 1=笔记本, 2=创建时间
    Q_INVOKABLE QVariant getFileInfo(const QString& cloudId);
    Q_INVOKABLE void downloadFile(const QString& cloudId, 
                                   const QString& targetPath, 
                                   const QString& notebookName,
                                   bool immediateMapping);
    
    // 记录查询
    Q_INVOKABLE QVariantList getUploadRecords(const QString& cloudId);
    Q_INVOKABLE QVariantList getPullRecords(const QString& cloudId);
    Q_INVOKABLE QVariantList getFileHistory(const QString& cloudId);
    
    // 文件版本
    Q_INVOKABLE void createNewVersion(const QString& cloudId, const QString& description);
    Q_INVOKABLE void downloadVersion(const QString& cloudId, int versionId, const QString& targetPath);
    // 路径选择
    Q_INVOKABLE QString explore(const QString& title = "选择路径");
    
    // 属性访问器
    QString networkStatus() const;
    bool isLoggedIn() const;
    QString accountName() const;
    QString userName() const;
    QString deviceRemark() const;
    void setDeviceRemark(const QString& remark);
    
signals:
    void networkStatusChanged();
    void loginStatusChanged(bool loggedIn);
    void accountInfoChanged();
    void deviceRemarkChanged();
    void fileListUpdated();
    void fileDownloaded(const QString& cloudId, bool success);
    void versionCreated(const QString& cloudId, bool success);
    
private:
    explicit CloudFileManagerAdapter(QObject* parent = nullptr);
    ~CloudFileManagerAdapter() = default;
    
    QString m_networkStatus;
    bool m_isLoggedIn = false;
    QString m_accountName="112233";
    QString m_userName="helloAcc";
    QString m_deviceRemark="test PC";


    inline static QVariantMap convertFileInfoToQVariant(const NetWorkFileInfo& fileInfo) {
        QVariantMap result;

        // 1. 文件名称 (QML中需要)
        result["fileName"] = fileInfo.fileName.isEmpty() ? "未命名文件" : fileInfo.fileName;

        // 2. 云端ID (QML中需要，但NetWorkFileInfo中没有cloudId字段，我们用fileID代替)
        result["cloudId"] = QString::number(fileInfo.fileID);

        // 3. 描述 (QML中需要，但NetWorkFileInfo中没有description字段，提供一个默认值)
        result["description"] = "这是一个文件";  // 默认描述

        // 4. 笔记本名称 (QML中需要，但NetWorkFileInfo中没有notebookName字段，提供一个默认值)
        result["notebookName"] = "默认笔记本";  // 默认笔记本名称

        // 5. 最后更新 (QML中需要，将updateTime转换为字符串)
        if (fileInfo.updateTime.isValid()) {
            result["lastUpdate"] = fileInfo.updateTime.toString("yyyy-MM-dd hh:mm:ss");
        } else {
            result["lastUpdate"] = "未知时间";
        }

        // 6. 创建时间 (虽然QML中没有直接使用，但可以添加进去)
        if (fileInfo.createTime.isValid()) {
            result["createTime"] = fileInfo.createTime.toString("yyyy-MM-dd hh:mm:ss");
        } else {
            result["createTime"] = "未知时间";
        }

        // 7. 文件ID (添加进去，虽然不是QML中需要的字段名)
        result["fileID"] = fileInfo.fileID;

        // 8. 最新版本ID (添加进去)
        result["latestVersionID"] = fileInfo.latestVersionID;

        return result;
    }
    static QVariantMap convertVersionInfoToQVariant(const NetWorkVersionInfo& versionInfo, int versionNumber = 0, const QString& versionName = "") {
        QVariantMap result;
        
        // 1. 版本ID (QML中需要为versionId，注意大小写)
        result["versionId"] = QString::number(versionInfo.versionID);
        
        // 2. 版本名称 (QML中需要，如果未提供则生成默认名称)
        if (!versionName.isEmpty()) {
            result["versionName"] = versionName;
        } else if (!versionInfo.description.isEmpty()) {
            // 使用描述作为版本名称
            result["versionName"] = versionInfo.description;
        } else {
            // 默认版本名称
            result["versionName"] = QString("版本 %1").arg(versionNumber > 0 ? versionNumber : 1);
        }
        
        // 3. 版本序号 (QML中需要为versionNumber)
        result["versionNumber"] = versionNumber > 0 ? versionNumber : 1;
        
        // 4. 创建时间 (QML中需要为createTime)
        if (versionInfo.createTime.isValid()) {
            result["createTime"] = versionInfo.createTime.toString("yyyy-MM-dd hh:mm:ss");
        } else {
            result["createTime"] = "未知时间";
        }
        
        // 5. 文件大小 (QML中需要为size，KB为单位，根据内容计算)
        if (!versionInfo.content.isEmpty()) {
            // 计算内容大小（字节数），转换为KB，保留2位小数
            double sizeKB = versionInfo.content.size() / 1024.0;
            result["size"] = QString::number(sizeKB, 'f', 2);
        } else {
            result["size"] = "0.00";
        }
        
        // 6. 与上一个版本差异 (QML中需要为difference，默认为0%)
        result["difference"] = "0%";  // 这是一个示例值，实际需要计算差异
        
        // 7. 描述 (添加进去，虽然不是QML中直接需要的字段)
        result["description"] = versionInfo.description.isEmpty() ? "无描述" : versionInfo.description;
        
        // 8. 文件ID (添加进去)
        result["fileID"] = versionInfo.fileID;
        
        return result;
    }
};
}


#endif // CLOUDFILEMANAGERADAPTER_H