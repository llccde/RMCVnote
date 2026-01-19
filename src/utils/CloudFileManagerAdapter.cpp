#include "cloudfilemanageradapter.h"
#include "CloudFileNetWork.h"
#include <qcollator.h>

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
    // Initialize the database
    CloudFileNetWork *network = CloudFileNetWork::getInstance();
    NetResult<bool> initResult = network->initializeDatabase();
    if (initResult.isFailure()) {
        qWarning() << "Failed to initialize database:" << initResult.status;
    }

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
    CloudFileNetWork *network = CloudFileNetWork::getInstance();
    NetResult<QList<NetWorkFileInfo>> resultData = network->getAllFiles((SortType)sortBy);

    QList<NetWorkFileInfo> sortedData;
    if (resultData.isSuccess()) {
        sortedData = *(resultData.data);
    } else {
        qWarning() << "Failed to get file list:" << resultData.status;
        // 返回空列表
    }

    // 转换为QVariantList返回
    QVariantList result;
    for (const auto &file : sortedData) {
        result.append(convertFileInfoToQVariant(file));
    }

    return result;
}




QVariant CloudFileManagerAdapter::getFileInfo(const QString& cloudId)
{
    CloudFileNetWork *network = CloudFileNetWork::getInstance();
    NetResult<NetWorkFileInfo> resultData = network->getFileByFileID(CloudFileNetWork::IDFromString(cloudId));

    if (resultData.isSuccess()) {
        NetWorkFileInfo fileData = *(resultData.data);
        return convertFileInfoToQVariant(fileData);
    } else {
        qWarning() << "Failed to get file info:" << resultData.status;
        // 返回空的QVariant
        return QVariant();
    }
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
    // For now, return empty list - in a real implementation this would query upload records
    Q_UNUSED(cloudId)
    return QVariantList();
}

QVariantList CloudFileManagerAdapter::getPullRecords(const QString& cloudId)
{
    // For now, return empty list - in a real implementation this would query pull/download records
    Q_UNUSED(cloudId)
    return QVariantList();
}

QVariantList CloudFileManagerAdapter::getFileHistory(const QString& cloudId)
{
    CloudFileNetWork *network = CloudFileNetWork::getInstance();
    NetResult<QList<NetWorkVersionInfo>> resultData = network->getAllVersions(CloudFileNetWork::IDFromString(cloudId));

    QList<NetWorkVersionInfo> versions;
    if (resultData.isSuccess()) {
        versions = *(resultData.data);
    } else {
        qWarning() << "Failed to get file history:" << resultData.status;
        // 返回空列表
    }

    QVariantList result;
    for (const auto &version : versions) {
        result.append(convertVersionInfoToQVariant(version));
    }

    return result;
}

void CloudFileManagerAdapter::createNewVersion(const QString& cloudId, const QString& description)
{
    CloudFileNetWork *network = CloudFileNetWork::getInstance();
    // For demo purposes, we'll use a default file size of 0, which will be calculated in the actual implementation
    NetResult<qint64> result = network->snapshotVersionForFile(CloudFileNetWork::IDFromString(cloudId), description);

    if (result.isSuccess()) {
        emit versionCreated(cloudId, true);
    } else {
        emit versionCreated(cloudId, false);
    }
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
QString CloudFileManagerAdapter::userName() const
{
    // 空实现，具体功能后续添加
    return m_userName;
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