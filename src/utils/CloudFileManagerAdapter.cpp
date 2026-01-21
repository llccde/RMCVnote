#include "cloudfilemanageradapter.h"
#include "CloudFileNetWork.h"
#include "LocalDataOfUser.h"
#include "notebook/node.h"
#include "notebook/notebook.h"
#include "vnotex.h"
#include <qcollator.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include "notebookmgr.h"
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
    m_accountName = username;
    m_isLoggedIn = true;
    emit loginStatusChanged(m_isLoggedIn);
}

void CloudFileManagerAdapter::logout()
{
    m_isLoggedIn = false;
    emit loginStatusChanged(m_isLoggedIn);
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
    // 实现下载文件功能
    CloudFileNetWork *network = CloudFileNetWork::getInstance();

    // 获取文件信息
    NetResult<NetWorkFileInfo> fileInfoResult = network->getFileByFileID(CloudFileNetWork::IDFromString(cloudId));

    if (fileInfoResult.isFailure()) {
        qWarning() << "Failed to get file info for cloudId:" << cloudId;
        emit fileDownloaded(cloudId, false);
        return;
    }

    NetWorkFileInfo fileInfo = *(fileInfoResult.data);

    // 获取最新版本内容
    NetResult<QString> contentResult = network->getVersionContent(fileInfo.latestVersionID);
    auto name = network->getFileByFileID(fileInfo.fileID);
    name.isNotError();
    if (contentResult.isFailure()) {
        qWarning() << "Failed to get content for file:" << cloudId;
        emit fileDownloaded(cloudId, false);
        return;
    }

    QString content = *(contentResult.data);
    


    // 根据参数决定保存位置
    if (targetPath.isEmpty()) {
        // 存储到笔记本
        QSharedPointer<Notebook> book;
        if (notebookName.isEmpty()) {
            book = VNoteX::getInst().getNotebookMgr().getCurrentNotebook();
        } else {
            book = VNoteX::getInst().getNotebookMgr().findNotebookWithGivenName(notebookName);
        }
        QSharedPointer<Node> node = book->newNode(book->getRootNode().data(), Node::Content, name.getData().fileName, content);
        emit book->nodeUpdated(node.data());
        if (immediateMapping) {
            auto mapping = LocalDataOfUser::getUser()->getMapping();
            (*mapping)[{book->getId(),node->getId()}] = cloudId;
        }
    } else {
        QString fullPath;
        // 存储到本地指定路径
        QDir dir(targetPath);
        if (!dir.exists()) {
            if (!dir.mkpath(targetPath)) {
                qWarning() << "Failed to create directory:" << targetPath;
                emit fileDownloaded(cloudId, false);
                return;
            }
        }

        fullPath = QDir(targetPath).filePath(fileInfo.fileName);
        QFile file(fullPath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << "Failed to open file for writing:" << fullPath;
            emit fileDownloaded(cloudId, false);
            return;
        }

        QTextStream out(&file);
        out.setEncoding(QStringConverter::Utf8);
        out << content;
        file.close();
    }

    // 写入文件


    // 发出信号通知下载完成
    emit fileDownloaded(cloudId, true);

    // 如果需要立即建立映射关系，这里可以添加相关逻辑

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

QVariantList CloudFileManagerAdapter::getNotebookList()
{

    QVariantList notebooks;
    auto books = VNoteX::getInst().getNotebookMgr().getNotebooks();
    for (auto i:books) {
        notebooks.append(i->getName());
    }
    return notebooks;
}

QString CloudFileManagerAdapter::explore(const QString& title)
{
    // 实现路径选择功能
    QFileDialog dialog;
    dialog.setWindowTitle(title);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);

    if (dialog.exec()) {
        QStringList selectedPaths = dialog.selectedFiles();
        if (!selectedPaths.isEmpty()) {
            return selectedPaths.first();
        }
    }

    return QString(); // 用户取消了对话框
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