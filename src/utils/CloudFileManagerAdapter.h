#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantList>
#include <QDateTime>

#ifndef CLOUDFILEMANAGERADAPTER_H
#define CLOUDFILEMANAGERADAPTER_H

namespace vnotex{

class CloudFileManagerAdapter : public QObject
{
    Q_OBJECT
    
    // 网络状况
    Q_PROPERTY(QString networkStatus READ networkStatus NOTIFY networkStatusChanged)
    
    // 登录状态
    Q_PROPERTY(bool isLoggedIn READ isLoggedIn NOTIFY loginStatusChanged)
    Q_PROPERTY(QString accountName READ accountName NOTIFY accountInfoChanged)
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
    QString m_accountName;
    QString m_deviceRemark;
};
}


#endif // CLOUDFILEMANAGERADAPTER_H