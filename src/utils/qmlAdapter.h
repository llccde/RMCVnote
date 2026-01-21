#ifndef QMLADAPTER_H
#define QMLADAPTER_H

#include "core/global.h"
#include "notebookmgr.h"
#include "vnotex.h"
#include <QObject>
#include <QVariant>
#include <QTimer>
#include <qcontainerfwd.h>
#include <qlist.h>
#include <qtmetamacros.h>
#include <qvariant.h>
#include <string>
#include "QVariantList"
namespace vnotex {

struct NoteVersionInfo {
    vnotex::ID version;
    QString time = "公园2233年";
    QString user = "默认用户";
    QString hash = "2233";
    bool isCurrent = false;
    qint64 size = 0;
    QString changeType;
    QString changeDescription;
    bool autoSave;

    inline QVariantMap toVariantMap() const {
        return {
            {"version", static_cast<qulonglong>(version)},
            {"time", time},
            {"user", user},
            {"hash", hash},
            {"isCurrent", isCurrent},
            {"size", size},
            {"changeType", changeType},
            {"changeDescription", changeDescription},
            {"autoSave", autoSave}
        };
    }
};

struct NoteDetailsInfo {
    enum syncStatus{
        notLatest = 0,
        isLatest = 1,
        unSync = 2,
        unknown = 3
    };
    QList<QString> conflictWith;
    vnotex::ID id;
    QString name;
    QString cloudId;
    QString filePath;
    qint64 size;
    QString modifiedTime;
    QString createdTime;
    syncStatus syncStatus;
    QString lastSyncTime;
    QString syncError;
    vnotex::ID currentVersion;
    vnotex::ID cloudVersion;
    bool hasConflict;
    QStringList tags;
    QStringList categories;
    bool isPinned;
    bool isEncrypted;
    int encryptionLevel;
    int wordCount;
    int readCount;
    int editCount;
    QList<NoteVersionInfo> versions;
    QVariantMap toVariantMap() const {
        QVariantMap map;
        map["id"] = static_cast<qulonglong>(id);
        map["name"] = name;
        map["cloudId"] = cloudId;
        map["filePath"] = filePath;
        map["size"] = size;
        map["modifiedTime"] = modifiedTime;
        map["createdTime"] = createdTime;
        map["syncStatus"] = syncStatus;
        map["lastSyncTime"] = lastSyncTime;
        map["syncError"] = syncError;
        map["currentVersion"] = static_cast<qulonglong>(currentVersion);
        map["cloudVersion"] = static_cast<qulonglong>(cloudVersion);
        map["hasConflict"] = hasConflict;
        map["tags"] = tags;
        map["categories"] = categories;
        map["isPinned"] = isPinned;
        map["isEncrypted"] = isEncrypted;
        map["encryptionLevel"] = encryptionLevel;
        map["wordCount"] = wordCount;
        map["readCount"] = readCount;
        map["editCount"] = editCount;
        auto vers = QVariantList();
        auto conflicts = QVariantList();
        for (auto i:versions) {
            vers.append(i.toVariantMap());
        }
        for (auto i : conflictWith) {
            conflicts.append(i);
        }
        map["versions"] = vers;
        map["conflictWith"] = conflicts;
        return map;
    }
};
class QmlAdapter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int backendStatus READ backendStatus WRITE setBackendStatus NOTIFY backendStatusChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage WRITE setStatusMessage NOTIFY statusMessageChanged)

public:
    explicit QmlAdapter(QObject *parent = nullptr);

    // Backend status枚举
    enum BackendStatus {
        Idle = 0,
        Processing = 1,
        Error = 2
    };
    Q_ENUM(BackendStatus)

    int backendStatus() const;
    void setBackendStatus(int status);

    QString statusMessage() const;
    void setStatusMessage(const QString &message);
    
    // 供QML调用的方法
    Q_INVOKABLE void refreshNotebooks();
    Q_INVOKABLE QVariantList getNotebooks() const;
    Q_INVOKABLE QVariantList getNotes(vnotex::ID notebookId) const;
    Q_INVOKABLE void addNotebook();
    Q_INVOKABLE void saveToCloud(vnotex::ID notebookId,vnotex::ID noteId);


    // 将文件写入到本地笔记
    Q_INVOKABLE void restoreNoteVersion(vnotex::ID noteId, vnotex::ID notebookId, vnotex::ID version);
    // 打开指定版本的笔记
    Q_INVOKABLE void openNoteVersion(vnotex::ID noteId, vnotex::ID notebookId, vnotex::ID version);
    // 获取详细信息
    Q_INVOKABLE QVariantMap getNoteDetails(vnotex::ID noteId, vnotex::ID notebookId);

    Q_INVOKABLE QVariantList getCloudNotes(vnotex::ID notebookId);
    Q_INVOKABLE QVariantList compareWithCloud(vnotex::ID notebookId);


    Q_INVOKABLE void syncAllNotebooks();
    Q_INVOKABLE void syncNotebook(vnotex::ID notebookId);
    Q_INVOKABLE void syncNote(vnotex::ID noteId, vnotex::ID notebookId);
    Q_INVOKABLE void openNote(vnotex::ID noteId, vnotex::ID notebookId);
    Q_INVOKABLE void openNoteInViewArea(vnotex::ID noteId, vnotex::ID notebookId);
    Q_INVOKABLE void snapshot(vnotex::ID notebookId, vnotex::ID noteId, const QString &description);
    Q_INVOKABLE void setSyncInterval(int minutes);
    Q_INVOKABLE int getSyncInterval() const;
    Q_INVOKABLE void setAutoSync(bool enabled);
    Q_INVOKABLE bool getAutoSync() const;
    Q_INVOKABLE void deleteVersion(vnotex::ID noteId, vnotex::ID notebookId, vnotex::ID version);
    Q_INVOKABLE void removeSync(vnotex::ID notebookId, vnotex::ID noteId);
    Q_INVOKABLE void getCloudFileName(vnotex::ID notebookId, vnotex::ID noteId);

signals:
    void backendStatusChanged(int status);
    void statusMessageChanged(const QString &message);
    void notebookListChanged(const QVariantList &notebooks);
    void noteListChanged(vnotex::ID notebookId, const QVariantList &notes);
    void noteDetailsChanged(vnotex::ID changedNoteId, vnotex::ID changedNotebookId);
    void syncProgressChanged(vnotex::ID notebookId, int progress);
    void syncErrorOccurred(const QString &error);
    void cloudConnectionChanged(bool connected);
    void noteChanged(vnotex::ID notebookId,vnotex::ID noteID);
    void cloudFileReturn(vnotex::ID notebookId, vnotex::ID noteId, const QString &cloudName);

    
private:
    void setupConnections();

    int m_backendStatus;
    QString m_statusMessage;
};

} // namespace vnotex

#endif // QMLADAPTER_H
