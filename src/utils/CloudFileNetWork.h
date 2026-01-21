#ifndef CLOUDFILENETWORK_H
#define CLOUDFILENETWORK_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QList>
#include <QVariant>
#include <QSharedPointer>
#include <qcontainerfwd.h>
#include <qlogging.h>
#include <qobject.h>
using CloudFileNetWorkFileAndVersionID = qint64;
// 错误状态枚举，定义所有可能的失败情况
enum ErrorStatus{
    Success = 1,                           // 操作成功
    Default = 0,                           // 默认值/未知错误
    FileIDNotFound = -1,                   // 文件ID未找到
    InvalidFileName = -2,                  // 无效的文件名（如空名称）
    DatabaseConnectionFailed = -3,         // 数据库连接失败
    DatabaseOpenFailed = -4,               // 数据库打开失败
    DatabaseQueryFailed = -5,              // 数据库查询失败
    DatabaseTransactionFailed = -6,        // 数据库事务失败
    DatabaseCreateTableFailed = -7,        // 创建表失败
    DatabaseDirectoryCreationFailed = -8,  // 数据库目录创建失败
    DatabaseFileRemovalFailed = -9,        // 数据库文件删除失败
    FileRenameFailed = -10,                // 文件重命名失败
    FileAdditionFailed = -11,              // 文件添加失败
    FileUpdateFailed = -12,                // 文件更新失败
    FileDeletionFailed = -13,              // 文件删除失败
    VersionCreationFailed = -14,           // 版本创建失败
    VersionRetrievalFailed = -15,          // 版本检索失败
    ContentUpdateFailed = -16,             // 内容更新失败
    TimeUpdateFailed = -17,                // 时间更新失败
    InvalidFileID = -18,                   // 无效的文件ID
    InvalidVersionID = -19,                // 无效的版本ID
    EmptyContent = -20,                    // 内容为空
    SqlExecutionError = -21,               // SQL执行错误
    TransactionRollbackError = -22,        // 事务回滚错误
    DirectoryPathCreationFailed = -23,     // 目录路径创建失败
    DatabaseNotInitialized = -24,          // 数据库未初始化
    LatestVersionNotFound = -25            // 最新版本未找到
};

// Result模板类，用于同时返回错误状态和数据
template<typename T>
class NetResult {
public:
    ErrorStatus status;
    QSharedPointer<T> data;

    // 构造函数：带状态和数据
    inline NetResult(ErrorStatus s, QSharedPointer<T> d) : status(s), data(d) {}

    // 构造函数：仅带状态（无数据）
    inline explicit NetResult(ErrorStatus s) : status(s), data(nullptr) {}

    // 构造函数：默认构造
    inline NetResult() : status(Default), data(nullptr) {}

    // 静态方法：创建成功的Result
    inline static NetResult<T> success(QSharedPointer<T> d) {
        return NetResult<T>(Success, d);
    }
    inline T getData(){
        return *data;
    }
    // 静态方法：创建失败的Result
    inline static NetResult<T> failure(ErrorStatus s) {
        return NetResult<T>(s, nullptr);
    }

    // 检查操作是否成功
    inline bool isSuccess() const {
        return status == Success;
    }
    //顺便输出错误信息
    inline bool isNotError() const{
        if(isSuccess()){
            return true;
        }else {
            qWarning()<<errorToString();
            return false;
        }
    }

    // 检查操作是否失败
    inline bool isFailure() const {
        return status != Success;
    }

    // 将错误状态转换为字符串描述
    inline QString errorToString() const {
        switch(status) {
            case Success:
                return "Success";
            case Default:
                return "Default/Unknown Error";
            case FileIDNotFound:
                return "File ID not found";
            case InvalidFileName:
                return "Invalid file name";
            case DatabaseConnectionFailed:
                return "Database connection failed";
            case DatabaseOpenFailed:
                return "Database open failed";
            case DatabaseQueryFailed:
                return "Database query failed";
            case DatabaseTransactionFailed:
                return "Database transaction failed";
            case DatabaseCreateTableFailed:
                return "Database create table failed";
            case DatabaseDirectoryCreationFailed:
                return "Database directory creation failed";
            case DatabaseFileRemovalFailed:
                return "Database file removal failed";
            case FileRenameFailed:
                return "File rename failed";
            case FileAdditionFailed:
                return "File addition failed";
            case FileUpdateFailed:
                return "File update failed";
            case FileDeletionFailed:
                return "File deletion failed";
            case VersionCreationFailed:
                return "Version creation failed";
            case VersionRetrievalFailed:
                return "Version retrieval failed";
            case ContentUpdateFailed:
                return "Content update failed";
            case TimeUpdateFailed:
                return "Time update failed";
            case InvalidFileID:
                return "Invalid file ID";
            case InvalidVersionID:
                return "Invalid version ID";
            case EmptyContent:
                return "Content is empty";
            case SqlExecutionError:
                return "SQL execution error";
            case TransactionRollbackError:
                return "Transaction rollback error";
            case DirectoryPathCreationFailed:
                return "Directory path creation failed";
            case DatabaseNotInitialized:
                return "Database not initialized";
            case LatestVersionNotFound:
                return "Latest version not found";
            default:
                return "Unknown error code";
        }
    }
};
// 排序类型枚举
enum SortType {
    SORT_BY_NAME_ASC = 0,    // 名称升序
    SORT_BY_NAME_DESC = 1,   // 名称降序
    SORT_BY_CREATE_TIME = 2  // 创建时间（默认按创建时间降序，最新在前）
};
// 版本文件信息结构体
struct NetWorkVersionInfo {
    qint64 versionID;
    qint64 fileID;
    QDateTime createTime;
    QString content;
    QString description;
    bool isCurrent = false;
    NetWorkVersionInfo() : versionID(0), fileID(0) {}
    NetWorkVersionInfo(qint64 vid, qint64 fid, const QDateTime& ct, 
                const QString& cnt, const QString& desc)
        : versionID(vid), fileID(fid), createTime(ct), content(cnt), description(desc) {}
};

// 文件信息结构体
struct NetWorkFileInfo {
    qint64 fileID;
    QString fileName;
    QDateTime updateTime;
    QDateTime createTime;
    qint64 latestVersionID;

    NetWorkFileInfo() : fileID(0), latestVersionID(0) {}
    NetWorkFileInfo(qint64 fid, const QString& name, const QDateTime& ut,
             const QDateTime& ct, qint64 lvid)
        : fileID(fid), fileName(name), updateTime(ut), createTime(ct), latestVersionID(lvid) {}
};


class CloudFileNetWork : public QObject
{
    Q_OBJECT

public:
    explicit CloudFileNetWork(QObject *parent = nullptr);
    ~CloudFileNetWork();

    // 获取单例实例
    static inline CloudFileNetWork* getInstance() {
        static CloudFileNetWork instance;
        return &instance;
    }
    CloudFileNetWorkFileAndVersionID static inline IDFromString(QString id){
        return id.toLongLong();
    }
    
    QString static inline IDToString(CloudFileNetWorkFileAndVersionID id){
      return QString::number(id);
    }
    // 初始化数据库连接
    NetResult<bool> initializeDatabase();

    // 重命名文件
    NetResult<bool> renameFile(qint64 fileID, const QString &newName);

    // 根据文件ID获取文件信息
    NetResult<NetWorkFileInfo> getFileByFileID(qint64 fileID);

    // 添加新文件，返回包含新文件ID的结果
    NetResult<qint64> addFile(const QString &fileName, const QString &initialContent = "",
                        const QString &description = "Initial version");

    // 为文件创建快照（复制当前最新版本），返回包含版本ID的结果
    NetResult<qint64> snapshotVersionForFile(qint64 fileID, const QString &description = "Snapshot");

    // 更新文件内容（直接写入最新版本）
    NetResult<bool> updateFileContent(qint64 fileID, const QString &newContent);

    // 获取文件的所有版本信息（不包含content）
    NetResult<QList<NetWorkVersionInfo>> getAllVersions(qint64 fileID);

    // 根据版本ID获取版本内容
    NetResult<QString> getVersionContent(qint64 versionID);

    // 获取所有文件列表
    NetResult<QList<NetWorkFileInfo>> getAllFiles(SortType sortBy = SORT_BY_CREATE_TIME);

    // 删除文件及其所有版本
    NetResult<bool> deleteFile(qint64 fileID);

    // 删除指定版本
    NetResult<bool> deleteFileVersion(qint64 fileID, qint64 versionID);

    // 获取文件的最新版本ID
    NetResult<qint64> getLatestVersionID(qint64 fileID);
private:
    QSqlDatabase m_database;

    // 创建数据库表
    NetResult<bool> createTables();

    // 获取当前时间戳字符串
    QString getCurrentTimestamp();

    
    

    // 更新文件的更新时间
    NetResult<bool> updateFileTime(qint64 fileID);

    // 创建新版本，通过引用参数返回版本ID（保持原有接口，因为这是内部方法）
    ErrorStatus createVersion(qint64 fileID, qint64 &versionID, const QString &content, const QString &description);

    //添加示例数据
    void addExampleData();
};

#endif // CLOUDFILENETWORK_H
