#include "CloudFileNetWork.h"
#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QStandardPaths>
#include <qdir.h>
#include <qstandardpaths.h>
CloudFileNetWork::CloudFileNetWork(QObject *parent)
    : QObject(parent)
{
}

CloudFileNetWork::~CloudFileNetWork()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

NetResult<bool> CloudFileNetWork::initializeDatabase()
{
    auto DBPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    
    // 使用固定的路径
    const QString fixedDbPath = QDir(DBPath).filePath("vnoteMock.db");

    // 确保目录存在
    QDir dir;
    if (!dir.mkpath(QFileInfo(fixedDbPath).path())) {
        qWarning() << "Failed to create database directory";
        return NetResult<bool>::failure(DatabaseDirectoryCreationFailed);
    }

    // 如果数据库文件已存在，则删除它
    QFile dbFile(fixedDbPath);
    if (dbFile.exists()) {
        if (!dbFile.remove()) {
            qWarning() << "Failed to remove existing database file:" << fixedDbPath;
            return NetResult<bool>::failure(DatabaseFileRemovalFailed);
        }
    }

    // 打开或创建数据库
    m_database = QSqlDatabase::addDatabase("QSQLITE", "cloud_file_connection");
    m_database.setDatabaseName(fixedDbPath);

    if (!m_database.open()) {
        qWarning() << "Failed to open database:" << m_database.lastError().text();
        return NetResult<bool>::failure(DatabaseOpenFailed);
    }

    // 创建表
    NetResult<bool> createTableResult = createTables();
    if (createTableResult.isFailure()) {
        return NetResult<bool>::failure(createTableResult.status);
    }

    if(true){
        //添加示例数据
        addExampleData();
    }

    return NetResult<bool>::success(QSharedPointer<bool>::create(true));
}

NetResult<bool> CloudFileNetWork::renameFile(CloudFileNetWorkFileAndVersionID fileID, const QString &newName)
{
    if (newName.trimmed().isEmpty()) {
        qWarning() << "File name cannot be empty";
        return NetResult<bool>::failure(FileRenameFailed);
    }

    QSqlQuery query(m_database);
    query.prepare("UPDATE File SET FileName = ?, UpdateTime = ? WHERE FileID = ?");
    query.addBindValue(newName);
    query.addBindValue(getCurrentTimestamp());
    query.addBindValue(fileID);

    if (!query.exec()) {
        qWarning() << "Failed to rename file:" << query.lastError().text();
        return NetResult<bool>::failure(SqlExecutionError);
    }

    if (query.numRowsAffected() == 0) {
        return NetResult<bool>::failure(FileRenameFailed);
    }

    return NetResult<bool>::success(QSharedPointer<bool>::create(true));
}

NetResult<NetWorkFileInfo> CloudFileNetWork::getFileByFileID(CloudFileNetWorkFileAndVersionID fileID)
{
    QSqlQuery query(m_database);
    query.prepare("SELECT FileID, FileName, UpdateTime, CreateTime, LatestVersionID FROM File WHERE FileID = ?");
    query.addBindValue(fileID);

    if (!query.exec()) {
        qWarning() << "Failed to get file by ID:" << query.lastError().text();
        return NetResult<NetWorkFileInfo>::failure(DatabaseQueryFailed);
    }

    if (!query.next()) {
        qWarning() << "File not found with ID:" << fileID;
        return NetResult<NetWorkFileInfo>::failure(FileIDNotFound);
    }

    NetWorkFileInfo fileInfo = NetWorkFileInfo(
        query.value(0).toLongLong(),
        query.value(1).toString(),
        QDateTime::fromString(query.value(2).toString(), Qt::ISODateWithMs),
        QDateTime::fromString(query.value(3).toString(), Qt::ISODateWithMs),
        query.value(4).toLongLong()
    );

    return NetResult<NetWorkFileInfo>::success(QSharedPointer<NetWorkFileInfo>::create(fileInfo));
}

NetResult<CloudFileNetWorkFileAndVersionID> CloudFileNetWork::addFile(const QString &fileName, const QString &initialContent, const QString &description)
{
    if (fileName.trimmed().isEmpty()) {
        qWarning() << "File name cannot be empty";
        return NetResult<CloudFileNetWorkFileAndVersionID>::failure(InvalidFileName);
    }

    QSqlQuery query(m_database);
    QString timestamp = getCurrentTimestamp();

    // 开始事务
    if (!m_database.transaction()) {
        qWarning() << "Failed to begin transaction";
        return NetResult<CloudFileNetWorkFileAndVersionID>::failure(DatabaseTransactionFailed);
    }

    try {
        // 插入文件记录
        query.prepare("INSERT INTO File (FileName, UpdateTime, CreateTime) VALUES (?, ?, ?)");
        query.addBindValue(fileName);
        query.addBindValue(timestamp);
        query.addBindValue(timestamp);

        if (!query.exec()) {
            throw std::runtime_error("Failed to insert file: " + query.lastError().text().toStdString());
        }

        CloudFileNetWorkFileAndVersionID fileID = query.lastInsertId().toLongLong();

        // 创建初始版本
        CloudFileNetWorkFileAndVersionID versionID;
        ErrorStatus versionResult = createVersion(fileID, versionID, initialContent, description);
        if (versionResult != Success) {
            throw std::runtime_error(("Failed to create initial version: " +
                                    QString::number(versionResult)).toStdString());
        }

        // 更新文件的最新版本ID
        query.prepare("UPDATE File SET LatestVersionID = ? WHERE FileID = ?");
        query.addBindValue(versionID);
        query.addBindValue(fileID);

        if (!query.exec()) {
            throw std::runtime_error("Failed to update latest version ID: " +
                                   query.lastError().text().toStdString());
        }

        if (!m_database.commit()) {
            throw std::runtime_error("Failed to commit transaction: " +
                                   m_database.lastError().text().toStdString());
        }

        return NetResult<CloudFileNetWorkFileAndVersionID>::success(QSharedPointer<CloudFileNetWorkFileAndVersionID>::create(fileID));

    } catch (const std::exception &e) {
        if (!m_database.rollback()) {
            qWarning() << "Failed to rollback transaction:" << m_database.lastError().text();
            return NetResult<CloudFileNetWorkFileAndVersionID>::failure(TransactionRollbackError);
        }
        qWarning() << "Failed to add file:" << e.what();
        return NetResult<CloudFileNetWorkFileAndVersionID>::failure(FileAdditionFailed);
    }
}

NetResult<CloudFileNetWorkFileAndVersionID> CloudFileNetWork::snapshotVersionForFile(CloudFileNetWorkFileAndVersionID fileID, const QString &description)
{
    // 获取最新版本的ID
    NetResult<CloudFileNetWorkFileAndVersionID> latestVersionResult = getLatestVersionID(fileID);
    if (latestVersionResult.isFailure()) {
        return NetResult<CloudFileNetWorkFileAndVersionID>::failure(latestVersionResult.status);
    }

    CloudFileNetWorkFileAndVersionID latestVersionID = *(latestVersionResult.data);
    if (latestVersionID <= 0) {
        qWarning() << "No latest version found for file ID:" << fileID;
        return NetResult<CloudFileNetWorkFileAndVersionID>::failure(LatestVersionNotFound);
    }

    // 获取最新版本的内容
    NetResult<QString> contentResult = getVersionContent(latestVersionID);
    if (contentResult.isFailure()) {
        return NetResult<CloudFileNetWorkFileAndVersionID>::failure(contentResult.status);
    }

    // 创建新版本
    CloudFileNetWorkFileAndVersionID versionID;
    ErrorStatus newVersionResult = createVersion(fileID, versionID, *(contentResult.data), description);
    if (newVersionResult != Success) {
        return NetResult<CloudFileNetWorkFileAndVersionID>::failure(newVersionResult);
    }

    if (versionID > 0) {
        // 更新文件的最新版本ID和更新时间
        QSqlQuery query(m_database);
        query.prepare("UPDATE File SET LatestVersionID = ?, UpdateTime = ? WHERE FileID = ?");
        query.addBindValue(versionID);
        query.addBindValue(getCurrentTimestamp());
        query.addBindValue(fileID);

        if (!query.exec()) {
            qWarning() << "Failed to update file after snapshot:" << query.lastError().text();
            return NetResult<CloudFileNetWorkFileAndVersionID>::failure(SqlExecutionError);
        }
    }

    return NetResult<CloudFileNetWorkFileAndVersionID>::success(QSharedPointer<CloudFileNetWorkFileAndVersionID>::create(versionID));
}

NetResult<bool> CloudFileNetWork::updateFileContent(CloudFileNetWorkFileAndVersionID fileID, const QString &newContent)
{
    NetResult<CloudFileNetWorkFileAndVersionID> latestVersionResult = getLatestVersionID(fileID);
    if (latestVersionResult.isFailure()) {
        return NetResult<bool>::failure(latestVersionResult.status);
    }

    CloudFileNetWorkFileAndVersionID latestVersionID = *(latestVersionResult.data);
    if (latestVersionID <= 0) {
        qWarning() << "No latest version found for file ID:" << fileID;
        return NetResult<bool>::failure(LatestVersionNotFound);
    }

    QSqlQuery query(m_database);
    query.prepare("UPDATE VersionFile SET Content = ? WHERE VersionID = ?");
    query.addBindValue(newContent);
    query.addBindValue(latestVersionID);

    if (!query.exec()) {
        qWarning() << "Failed to update file content:" << query.lastError().text();
        return NetResult<bool>::failure(SqlExecutionError);
    }

    // 更新文件的更新时间
    NetResult<bool> updateTimeResult = updateFileTime(fileID);
    return updateTimeResult;
}

NetResult<QList<NetWorkVersionInfo>> CloudFileNetWork::getAllVersions(CloudFileNetWorkFileAndVersionID fileID)
{
    QList<NetWorkVersionInfo> versions; // 创建局部变量

    QSqlQuery query(m_database);
    query.prepare("SELECT VersionID, FileID, CreateTime, Description FROM VersionFile WHERE FileID = ? ORDER BY CreateTime DESC");
    query.addBindValue(fileID);

    if (!query.exec()) {
        qWarning() << "Failed to get versions:" << query.lastError().text();
        return NetResult<QList<NetWorkVersionInfo>>::failure(VersionRetrievalFailed);
    }

    while (query.next()) {
        NetWorkVersionInfo info(
            query.value(0).toLongLong(),
            query.value(1).toLongLong(),
            QDateTime::fromString(query.value(2).toString(), Qt::ISODateWithMs),
            "",  // content为空，不在AllVersion中返回
            query.value(3).toString()
        );
        versions.append(info);
    }

    return NetResult<QList<NetWorkVersionInfo>>::success(QSharedPointer<QList<NetWorkVersionInfo>>::create(versions));
}

NetResult<QString> CloudFileNetWork::getVersionContent(CloudFileNetWorkFileAndVersionID versionID)
{
    QSqlQuery query(m_database);
    query.prepare("SELECT Content FROM VersionFile WHERE VersionID = ?");
    query.addBindValue(versionID);

    if (!query.exec()) {
        qWarning() << "Failed to get version content:" << query.lastError().text();
        return NetResult<QString>::failure(DatabaseQueryFailed);
    }

    if (!query.next()) {
        qWarning() << "Version not found with ID:" << versionID;
        return NetResult<QString>::failure(InvalidVersionID);
    }

    QString content = query.value(0).toString();
    return NetResult<QString>::success(QSharedPointer<QString>::create(content));
}

NetResult<QList<NetWorkFileInfo>> CloudFileNetWork::getAllFiles(SortType sortBy)
{
    QList<NetWorkFileInfo> files; // 创建局部变量

    QSqlQuery query(m_database);
    QString sql = "SELECT FileID, FileName, UpdateTime, CreateTime, LatestVersionID FROM File ";

    // 根据排序类型添加ORDER BY子句
    switch (sortBy) {
    case SORT_BY_NAME_ASC:
        sql += "ORDER BY FileName ASC";
        break;
    case SORT_BY_NAME_DESC:
        sql += "ORDER BY FileName DESC";
        break;
    case SORT_BY_CREATE_TIME:
    default:
        sql += "ORDER BY CreateTime DESC"; // 创建时间降序，最新创建的在前
        break;
    }

    if (!query.exec(sql)) {
        qWarning() << "Failed to get all files:" << query.lastError().text();
        return NetResult<QList<NetWorkFileInfo>>::failure(DatabaseQueryFailed);
    }

    while (query.next()) {
        NetWorkFileInfo info(
            query.value(0).toLongLong(),
            query.value(1).toString(),
            QDateTime::fromString(query.value(2).toString(), Qt::ISODateWithMs),
            QDateTime::fromString(query.value(3).toString(), Qt::ISODateWithMs),
            query.value(4).toLongLong()
        );
        files.append(info);
    }

    return NetResult<QList<NetWorkFileInfo>>::success(QSharedPointer<QList<NetWorkFileInfo>>::create(files));
}

NetResult<bool> CloudFileNetWork::deleteFile(CloudFileNetWorkFileAndVersionID fileID)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM File WHERE FileID = ?");
    query.addBindValue(fileID);

    if (!query.exec()) {
        qWarning() << "Failed to delete file:" << query.lastError().text();
        return NetResult<bool>::failure(SqlExecutionError);
    }

    if (query.numRowsAffected() == 0) {
        return NetResult<bool>::failure(FileDeletionFailed);
    }

    return NetResult<bool>::success(QSharedPointer<bool>::create(true));
}

NetResult<bool> CloudFileNetWork::deleteFileVersion(CloudFileNetWorkFileAndVersionID fileID, CloudFileNetWorkFileAndVersionID versionID)
{
    // 首先检查版本是否存在且属于指定文件
    QSqlQuery checkQuery(m_database);
    checkQuery.prepare("SELECT VersionID FROM VersionFile WHERE VersionID = ? AND FileID = ?");
    checkQuery.addBindValue(versionID);
    checkQuery.addBindValue(fileID);

    if (!checkQuery.exec() || !checkQuery.next()) {
        qWarning() << "Version not found or does not belong to file:" << versionID << "File:" << fileID;
        return NetResult<bool>::failure(VersionRetrievalFailed);
    }

    // 检查是否是最新版本，不允许删除最新版本
    NetResult<CloudFileNetWorkFileAndVersionID> latestVersionResult = getLatestVersionID(fileID);
    if (latestVersionResult.isFailure()) {
        qWarning() << "Failed to get latest version ID for file:" << fileID;
        return NetResult<bool>::failure(LatestVersionNotFound);
    }

    CloudFileNetWorkFileAndVersionID latestVersionID = *(latestVersionResult.data);
    if (versionID == latestVersionID) {
        qWarning() << "Cannot delete latest version:" << versionID;
        return NetResult<bool>::failure(VersionRetrievalFailed); // 使用合适的错误码
    }

    // 删除指定版本
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM VersionFile WHERE VersionID = ?");
    query.addBindValue(versionID);

    if (!query.exec()) {
        qWarning() << "Failed to delete version:" << query.lastError().text();
        return NetResult<bool>::failure(SqlExecutionError);
    }

    if (query.numRowsAffected() == 0) {
        return NetResult<bool>::failure(VersionRetrievalFailed);
    }

    return NetResult<bool>::success(QSharedPointer<bool>::create(true));
}

NetResult<bool> CloudFileNetWork::createTables()
{
    QSqlQuery query(m_database);

    // 创建文件表
    QString createFileTable = R"(
        CREATE TABLE IF NOT EXISTS File (
            FileID INTEGER PRIMARY KEY AUTOINCREMENT,
            FileName TEXT NOT NULL,
            UpdateTime TEXT NOT NULL,
            CreateTime TEXT NOT NULL,
            LatestVersionID INTEGER DEFAULT 0
        )
    )";

    if (!query.exec(createFileTable)) {
        qWarning() << "Failed to create File table:" << query.lastError().text();
        return NetResult<bool>::failure(DatabaseCreateTableFailed);
    }

    // 创建版本文件表
    QString createVersionTable = R"(
        CREATE TABLE IF NOT EXISTS VersionFile (
            VersionID INTEGER PRIMARY KEY AUTOINCREMENT,
            FileID INTEGER NOT NULL,
            CreateTime TEXT NOT NULL,
            Content TEXT,
            Description TEXT,
            FOREIGN KEY (FileID) REFERENCES File(FileID) ON DELETE CASCADE
        )
    )";

    if (!query.exec(createVersionTable)) {
        qWarning() << "Failed to create VersionFile table:" << query.lastError().text();
        return NetResult<bool>::failure(DatabaseCreateTableFailed);
    }

    // 创建索引以提高查询性能
    query.exec("CREATE INDEX IF NOT EXISTS idx_file_id ON VersionFile(FileID)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_version_id ON VersionFile(VersionID)");

    return NetResult<bool>::success(QSharedPointer<bool>::create(true));
}

QString CloudFileNetWork::getCurrentTimestamp()
{
    return QDateTime::currentDateTime().toString(Qt::ISODateWithMs);
}

NetResult<CloudFileNetWorkFileAndVersionID> CloudFileNetWork::getLatestVersionID(CloudFileNetWorkFileAndVersionID fileID)
{
    QSqlQuery query(m_database);
    query.prepare("SELECT LatestVersionID FROM File WHERE FileID = ?");
    query.addBindValue(fileID);

    if (!query.exec()) {
        qWarning() << "Failed to get latest version ID:" << query.lastError().text();
        return NetResult<CloudFileNetWorkFileAndVersionID>::failure(DatabaseQueryFailed);
    }

    if (!query.next()) {
        qWarning() << "File not found with ID:" << fileID;
        return NetResult<CloudFileNetWorkFileAndVersionID>::failure(FileIDNotFound);
    }

    CloudFileNetWorkFileAndVersionID versionID = query.value(0).toLongLong();
    if (versionID <= 0) {
        return NetResult<CloudFileNetWorkFileAndVersionID>::failure(LatestVersionNotFound);
    }

    return NetResult<CloudFileNetWorkFileAndVersionID>::success(QSharedPointer<CloudFileNetWorkFileAndVersionID>::create(versionID));
}

NetResult<bool> CloudFileNetWork::updateFileTime(CloudFileNetWorkFileAndVersionID fileID)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE File SET UpdateTime = ? WHERE FileID = ?");
    query.addBindValue(getCurrentTimestamp());
    query.addBindValue(fileID);

    if (!query.exec()) {
        qWarning() << "Failed to update file time:" << query.lastError().text();
        return NetResult<bool>::failure(TimeUpdateFailed);
    }

    if (query.numRowsAffected() == 0) {
        return NetResult<bool>::failure(TimeUpdateFailed);
    }

    return NetResult<bool>::success(QSharedPointer<bool>::create(true));
}

ErrorStatus CloudFileNetWork::createVersion(CloudFileNetWorkFileAndVersionID fileID, CloudFileNetWorkFileAndVersionID &versionID, const QString &content, const QString &description)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO VersionFile (FileID, CreateTime, Content, Description) VALUES (?, ?, ?, ?)");
    query.addBindValue(fileID);
    query.addBindValue(getCurrentTimestamp());
    query.addBindValue(content);
    query.addBindValue(description);

    if (!query.exec()) {
        qWarning() << "Failed to create version:" << query.lastError().text();
        return VersionCreationFailed;
    }

    versionID = query.lastInsertId().toLongLong();
    if (versionID <= 0) {
        return VersionCreationFailed;
    }

    return Success;
}

void CloudFileNetWork::addExampleData()
{
    // 检查数据库是否已打开
    if (!m_database.isOpen()) {
        qWarning() << "Database is not open";
        return;
    }

    qDebug() << "开始插入示例数据...";

    try {
        // 示例内容模板
        QString markdownContentTemplate = R"(
# %1

## 创建时间
%2

## 描述
%3

## 内容
这是一个Markdown文档示例，用于演示文件版本管理功能。

### 功能特点
1. 版本控制
2. 内容追踪
3. 历史回溯
4. 差异比较

### 使用说明
- 这是第%4个版本
- 文档类型：Markdown
- 文件ID：%5
- 版本ID：自动生成

---
**更新日志：**
- 创建了此文档
- 添加了基本结构
- 完善了文档内容
)";

        // 示例文件名
        QStringList fileNames = {
            "项目计划.md",
            "技术文档.md",
            "会议记录.md",
            "开发日志.md",
            "API参考.md"
        };

        // 示例描述
        QStringList descriptions = {
            "初始版本",
            "更新内容结构",
            "修复拼写错误",
            "添加新章节",
            "优化格式",
            "增加示例代码",
            "更新API文档",
            "添加用户指南",
            "修复链接问题",
            "更新版本号",
            "增加插图",
            "优化性能说明",
            "添加常见问题",
            "更新联系方式",
            "最终版本"
        };

        // 插入5个文件
        for (int i = 0; i < 5; ++i) {
            // 为每个文件生成不同的初始内容
            QString initialContent = markdownContentTemplate
                .arg(fileNames[i])
                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                .arg("这是" + fileNames[i] + "的初始版本")
                .arg("1")
                .arg(QString::number(i + 1));

            // 添加文件
            NetResult<CloudFileNetWorkFileAndVersionID> fileResult = addFile(fileNames[i], initialContent, descriptions[i % descriptions.size()]);

            if (fileResult.isSuccess() && *(fileResult.data) > 0) {
                CloudFileNetWorkFileAndVersionID fileID = *(fileResult.data);
                qDebug() << "添加文件成功:" << fileNames[i] << "FileID:" << fileID;

                // 每个文件添加额外版本（总共每个文件有3个版本）
                for (int versionNum = 2; versionNum <= 3; ++versionNum) {
                    // 为每个新版本生成不同的内容
                    QString newContent = markdownContentTemplate
                        .arg(fileNames[i])
                        .arg(QDateTime::currentDateTime().addSecs(versionNum * 3600).toString("yyyy-MM-dd hh:mm:ss"))
                        .arg("这是" + fileNames[i] + "的第" + QString::number(versionNum) + "个版本")
                        .arg(QString::number(versionNum))
                        .arg(QString::number(fileID));

                    // 使用快照方式添加新版本
                    NetResult<CloudFileNetWorkFileAndVersionID> versionResult = snapshotVersionForFile(fileID,
                        descriptions[(i * 3 + versionNum - 1) % descriptions.size()]);

                    if (versionResult.isSuccess() && *(versionResult.data) > 0) {
                        CloudFileNetWorkFileAndVersionID versionID = *(versionResult.data);
                        // 更新新版本的内容
                        QSqlQuery query(m_database);
                        query.prepare("UPDATE VersionFile SET Content = ? WHERE VersionID = ?");
                        query.addBindValue(newContent);
                        query.addBindValue(versionID);

                        if (!query.exec()) {
                            qWarning() << "更新版本内容失败:" << query.lastError().text();
                        }

                        qDebug() << "  添加版本:" << versionNum << "VersionID:" << versionID;
                    } else {
                        qWarning() << "创建版本失败 for file" << fileNames[i];
                    }
                }
            } else {
                qWarning() << "添加文件失败:" << fileNames[i];
            }
        }

        qDebug() << "示例数据插入完成!";
        qDebug() << "文件数量: 5";
        qDebug() << "版本总数: 15 (每个文件3个版本)";

        // 验证数据
        NetResult<QList<NetWorkFileInfo>> allFilesResult = getAllFiles(SORT_BY_CREATE_TIME);
        if (allFilesResult.isSuccess()) {
            QList<NetWorkFileInfo> allFiles = *(allFilesResult.data);
            qDebug() << "实际文件数量:" << allFiles.size();

            int totalVersions = 0;
            for (const NetWorkFileInfo& file : allFiles) {
                NetResult<QList<NetWorkVersionInfo>> versionsResult = getAllVersions(file.fileID);
                if (versionsResult.isSuccess()) {
                    QList<NetWorkVersionInfo> versions = *(versionsResult.data);
                    totalVersions += versions.size();
                    qDebug() << "文件" << file.fileID << file.fileName << "有" << versions.size() << "个版本";
                } else {
                    qWarning() << "获取版本失败 for file" << file.fileID;
                }
            }
            qDebug() << "实际版本总数:" << totalVersions;
        } else {
            qWarning() << "获取所有文件失败";
        }

    } catch (const std::exception &e) {
        qWarning() << "插入示例数据失败:" << e.what();
    }
}