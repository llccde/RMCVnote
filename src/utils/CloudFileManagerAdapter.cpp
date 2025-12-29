#include "cloudfilemanageradapter.h"

using namespace vnotex;

CloudFileManagerAdapter* CloudFileManagerAdapter::getAdapter()
{
    // 空实现，具体功能后续添加
    static CloudFileManagerAdapter adapter(nullptr);
    return &adapter;
}

CloudFileManagerAdapter::CloudFileManagerAdapter(QObject *parent)
    : QObject(parent)
{
    // 空实现，具体功能后续添加
}

void CloudFileManagerAdapter::login(const QString& username, const QString& password, bool remember)
{
    // 空实现，具体功能后续添加
    Q_UNUSED(username)
    Q_UNUSED(password)
    Q_UNUSED(remember)
}

void CloudFileManagerAdapter::logout()
{
    // 空实现，具体功能后续添加
}

QVariantList CloudFileManagerAdapter::getFileList(int sortBy)
{
    // 空实现，具体功能后续添加
    Q_UNUSED(sortBy)
    return QVariantList();
}

QVariant CloudFileManagerAdapter::getFileInfo(const QString& cloudId)
{
    // 空实现，具体功能后续添加
    Q_UNUSED(cloudId)
    return QVariant();
}

void CloudFileManagerAdapter::downloadFile(const QString& cloudId,
                                           const QString& targetPath,
                                           const QString& notebookName,
                                           bool immediateMapping)
{
    // 空实现，具体功能后续添加
    Q_UNUSED(cloudId)
    Q_UNUSED(targetPath)
    Q_UNUSED(notebookName)
    Q_UNUSED(immediateMapping)
}

QVariantList CloudFileManagerAdapter::getUploadRecords(const QString& cloudId)
{
    // 空实现，具体功能后续添加
    Q_UNUSED(cloudId)
    return QVariantList();
}

QVariantList CloudFileManagerAdapter::getPullRecords(const QString& cloudId)
{
    // 空实现，具体功能后续添加
    Q_UNUSED(cloudId)
    return QVariantList();
}

QVariantList CloudFileManagerAdapter::getFileHistory(const QString& cloudId)
{
    // 空实现，具体功能后续添加
    Q_UNUSED(cloudId)
    return QVariantList();
}

void CloudFileManagerAdapter::createNewVersion(const QString& cloudId, const QString& description)
{
    // 空实现，具体功能后续添加
    Q_UNUSED(cloudId)
    Q_UNUSED(description)
}

void CloudFileManagerAdapter::downloadVersion(const QString& cloudId, int versionId, const QString& targetPath)
{
    // 空实现，具体功能后续添加
    Q_UNUSED(cloudId)
    Q_UNUSED(versionId)
    Q_UNUSED(targetPath)
}

QString CloudFileManagerAdapter::explore(const QString& title)
{
    // 空实现，具体功能后续添加
    Q_UNUSED(title)
    return QString();
}

QString CloudFileManagerAdapter::networkStatus() const
{
    // 空实现，具体功能后续添加
    return m_networkStatus;
}

bool CloudFileManagerAdapter::isLoggedIn() const
{
    // 空实现，具体功能后续添加
    return m_isLoggedIn;
}

QString CloudFileManagerAdapter::accountName() const
{
    // 空实现，具体功能后续添加
    return m_accountName;
}

QString CloudFileManagerAdapter::deviceRemark() const
{
    // 空实现，具体功能后续添加
    return m_deviceRemark;
}

void CloudFileManagerAdapter::setDeviceRemark(const QString& remark)
{
    // 空实现，具体功能后续添加
    if (m_deviceRemark != remark) {
        m_deviceRemark = remark;
        emit deviceRemarkChanged();
    }
}