#ifndef QMLADAPTER_H
#define QMLADAPTER_H

#include "notebookmgr.h"
#include "vnotex.h"
#include <QObject>
#include <QVariant>
#include <QTimer>

namespace vnotex {

struct NoteVersionInfo {
    int version;
    QString time;
    QString user;
    QString hash;
    bool isCurrent;
    qint64 size;
    QString changeType;
    QString changeDescription;
    bool autoSave;
    
    inline QVariantMap toVariantMap() const {
        return {
            {"version", version},
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
    int id;
    QString name;
    QString cloudId;
    QString filePath;
    qint64 size;
    QString modifiedTime;
    QString createdTime;
    int syncStatus;
    QString lastSyncTime;
    QString syncError;
    int currentVersion;
    int cloudVersion;
    bool hasConflict;
    QStringList tags;
    QStringList categories;
    bool isPinned;
    bool isEncrypted;
    int encryptionLevel;
    int wordCount;
    int readCount;
    int editCount;
    
    QVariantMap toVariantMap() const {
        QVariantMap map;
        map["id"] = id;
        map["name"] = name;
        map["cloudId"] = cloudId;
        map["filePath"] = filePath;
        map["size"] = size;
        map["modifiedTime"] = modifiedTime;
        map["createdTime"] = createdTime;
        map["syncStatus"] = syncStatus;
        map["lastSyncTime"] = lastSyncTime;
        map["syncError"] = syncError;
        map["currentVersion"] = currentVersion;
        map["cloudVersion"] = cloudVersion;
        map["hasConflict"] = hasConflict;
        map["tags"] = tags;
        map["categories"] = categories;
        map["isPinned"] = isPinned;
        map["isEncrypted"] = isEncrypted;
        map["encryptionLevel"] = encryptionLevel;
        map["wordCount"] = wordCount;
        map["readCount"] = readCount;
        map["editCount"] = editCount;
        return map;
    }
};
class QmlAdapter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString syncStatus READ syncStatus WRITE setSyncStatus NOTIFY syncStatusChanged)
    
public:
    explicit QmlAdapter(QObject *parent = nullptr);
    
    QString syncStatus() const;
    void setSyncStatus(const QString &status);
    
    // 供QML调用的方法
    Q_INVOKABLE void refreshNotebooks();
    Q_INVOKABLE QVariantList getNotebooks() const;
    Q_INVOKABLE QVariantList getNotes(int notebookId) const;
    Q_INVOKABLE void addNotebook();
    
    // 在编辑器中打开给定笔记的给定版本
    Q_INVOKABLE void viewNoteVersion(int noteId, int notebookId, int version);
    // 将其下载到本地,但是不打开
    Q_INVOKABLE void restoreNoteVersion(int noteId, int notebookId, int version);
    // 获取详细信息
    Q_INVOKABLE QVariantMap getNoteDetails(int noteId, int notebookId);
    
    Q_INVOKABLE QVariantList getCloudNotes(int notebookId);
    Q_INVOKABLE QVariantList compareWithCloud(int notebookId);


    Q_INVOKABLE void syncAllNotebooks();
    Q_INVOKABLE void syncNotebook(int notebookId);
    Q_INVOKABLE void syncNote(int noteId, int notebookId);
    Q_INVOKABLE void openNote(int noteId, int notebookId);
    Q_INVOKABLE void setSyncInterval(int minutes);
    Q_INVOKABLE int getSyncInterval() const;
    Q_INVOKABLE void setAutoSync(bool enabled);
    Q_INVOKABLE bool getAutoSync() const;
    
signals:
    void syncStatusChanged(const QString &status);
    void notebookListChanged(const QVariantList &notebooks);
    void noteListChanged(int notebookId, const QVariantList &notes);
    void noteDetailsChanged(int noteId, int notebookId);
    void syncProgressChanged(int notebookId, int progress);
    void syncErrorOccurred(const QString &error);
    void cloudConnectionChanged(bool connected);
    
private:
    void setupConnections();
    
    QString m_syncStatus;
};

} // namespace vnotex

#endif // QMLADAPTER_H