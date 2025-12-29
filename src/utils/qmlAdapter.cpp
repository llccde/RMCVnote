#include "qmladapter.h"
#include "notebook/notebook.h"
#include "notebookmgr.h"
#include "vnotex.h"
#include <QTimer>
#include <QDebug>

using namespace vnotex;

QmlAdapter::QmlAdapter(QObject *parent)
    : QObject(parent),
      m_syncStatus("idle")
{
    setupConnections();
}

void QmlAdapter::setupConnections()
{
    // 连接笔记本管理器信号
    connect(&VNoteX::getInst().getNotebookMgr(), &NotebookMgr::notebooksUpdated, this, [this]() {
        for (auto i : VNoteX::getInst().getNotebookMgr().getNotebooks()) {
            connect(i.data(), &Notebook::nodeUpdated, [this](const Node *p) {
                emit noteListChanged(p->getNotebook()->getId(), 
                                    getNotes(p->getNotebook()->getId()));
            });
        }
        emit notebookListChanged(getNotebooks());
    });
    
    connect(&VNoteX::getInst().getNotebookMgr(), &NotebookMgr::notebookUpdated, 
            this, [this](const Notebook* book) {
                connect(book, &Notebook::nodeUpdated,
                    [this](const Node *p) {
                        emit notebookListChanged(getNotebooks());
                    });
            });
    
    // 初始发射笔记本列表
    emit notebookListChanged(getNotebooks());
}

QString QmlAdapter::syncStatus() const {
    return m_syncStatus;
}

void QmlAdapter::setSyncStatus(const QString &status) {
    if (m_syncStatus != status) {
        m_syncStatus = status;
        emit syncStatusChanged(status);
    }
}

// 刷新笔记本列表
void QmlAdapter::refreshNotebooks() {
    qDebug() << "Refreshing notebooks...";
    setSyncStatus("syncing");
    
    // 获取笔记本列表
    QVariantList notebooks = getNotebooks();
    emit notebookListChanged(notebooks);
    
    QTimer::singleShot(1000, this, [this]() {
        setSyncStatus("idle");
    });
}

// 获取所有笔记本
QVariantList QmlAdapter::getNotebooks() const {
    QVariantList result;

    const auto &notebooks = VNoteX::getInst().getNotebookMgr().getNotebooks();
    for (const auto &notebook : notebooks) {
        QVariantMap notebookMap;
        notebookMap["id"] = notebook->getId();
        notebookMap["name"] = notebook->getName();
        notebookMap["description"] = notebook->getDescription();
        notebookMap["type"] = notebook->getType();
        result.append(notebookMap);
    }
    
    return result;
}

// 获取指定笔记本的所有笔记
QVariantList QmlAdapter::getNotes(int notebookId) const {
    QVariantList result;
    
    auto notebook = VNoteX::getInst().getNotebookMgr().findNotebookById(notebookId);
    if (!notebook) {
        qWarning() << "Notebook not found:" << notebookId;
        return result;
    }
    
    // 获取根节点
    auto rootNode = notebook->getRootNode();
    if (!rootNode) {
        qWarning() << "Root node not found";
        return result;
    }
    
    // 递归获取所有有内容的节点
    std::function<void(const QSharedPointer<Node>&)> collectNotes;
    collectNotes = [&](const QSharedPointer<Node>& node) {
        if (node->hasContent()) {
            QVariantMap noteMap;
            noteMap["id"] = node->getId();
            noteMap["name"] = node->getName();
            noteMap["path"] = node->fetchPath();
            result.append(noteMap);
        }
        
        // 递归处理子节点
        const auto &children = node->getChildrenRef();
        for (const auto &child : children) {
            collectNotes(child);
        }
    };
    
    collectNotes(rootNode);
    
    return result;
}

// 同步所有笔记本
void QmlAdapter::syncAllNotebooks() {
    qDebug() << "Syncing all notebooks...";
    setSyncStatus("syncing");
    
    // 模拟同步过程
    QTimer::singleShot(2000, this, [this]() {
        setSyncStatus("success");
        
        // 刷新显示
        refreshNotebooks();
    });
}

// 同步单个笔记本
void QmlAdapter::syncNotebook(int notebookId) {
    qDebug() << "Syncing notebook:" << notebookId;
    setSyncStatus("syncing");
    
    QTimer::singleShot(1500, this, [this, notebookId]() {
        setSyncStatus("success");
        qDebug() << "Notebook" << notebookId << "synced successfully";
    });
}

// 同步单个笔记
void QmlAdapter::syncNote(int noteId, int notebookId) {
    qDebug() << "Syncing note:" << noteId << "from notebook:" << notebookId;
    setSyncStatus("syncing");
    
    QTimer::singleShot(1000, this, [this]() {
        setSyncStatus("success");
    });
}

// 打开笔记
void QmlAdapter::openNote(int noteId, int notebookId) {
    qDebug() << "Opening note:" << noteId << "from notebook:" << notebookId;
}

// 添加笔记本
void QmlAdapter::addNotebook() {
    qDebug() << "Add notebook clicked";
}

// 查看笔记版本
void QmlAdapter::viewNoteVersion(int noteId, int notebookId, int version) {
    Q_UNUSED(noteId)
    Q_UNUSED(notebookId)
    Q_UNUSED(version)
    qDebug() << "Viewing note version:" << noteId << "from notebook:" << notebookId << "version:" << version;
}

// 恢复笔记版本
void QmlAdapter::restoreNoteVersion(int noteId, int notebookId, int version) {
    Q_UNUSED(noteId)
    Q_UNUSED(notebookId)
    Q_UNUSED(version)
    qDebug() << "Restoring note version:" << noteId << "from notebook:" << notebookId << "version:" << version;
}

// 获取笔记详细信息
QVariantMap QmlAdapter::getNoteDetails(int noteId, int notebookId) {
    qDebug() << "Getting note details:" << noteId << "from notebook:" << notebookId;

    // 实际应该从数据层获取，这里返回模拟数据
    NoteDetailsInfo details;
    details.id = noteId;
    details.name = QString("笔记 %1").arg(noteId);
    details.cloudId = QString("CLOUD-%1-%2").arg(notebookId).arg(noteId);
    details.filePath = QString("/notes/note_%1.md").arg(noteId);
    details.size = 2048; // 2KB
    details.modifiedTime = "2023-10-15 14:30:22";
    details.createdTime = "2023-10-13 16:45:10";
    details.syncStatus = 1; // 1:已同步
    details.lastSyncTime = "2023-10-15 14:35:00";
    details.syncError = "";
    details.currentVersion = 3;
    details.cloudVersion = 3;
    details.hasConflict = false;
    details.tags = QStringList() << "工作" << "重要";
    details.categories = QStringList() << "技术文档";
    details.isPinned = true;
    details.isEncrypted = false;
    details.encryptionLevel = 0;
    details.wordCount = 1250;
    details.readCount = 15;
    details.editCount = 8;

    return details.toVariantMap();
}

// 获取云端笔记列表
QVariantList QmlAdapter::getCloudNotes(int notebookId) {
    Q_UNUSED(notebookId)
    qDebug() << "Getting cloud notes for notebook:" << notebookId;

    // 返回空列表
    return QVariantList();
}

// 与云端比较
QVariantList QmlAdapter::compareWithCloud(int notebookId) {
    Q_UNUSED(notebookId)
    qDebug() << "Comparing with cloud for notebook:" << notebookId;

    // 返回空比较结果
    return QVariantList();
}

// 设置同步间隔
void QmlAdapter::setSyncInterval(int minutes) {
    Q_UNUSED(minutes)
    qDebug() << "Setting sync interval to:" << minutes << "minutes";
}

// 获取同步间隔
int QmlAdapter::getSyncInterval() const {
    qDebug() << "Getting sync interval";
    // 返回默认值，例如30分钟
    return 30;
}

// 设置自动同步
void QmlAdapter::setAutoSync(bool enabled) {
    qDebug() << "Setting auto sync to:" << enabled;
}

// 获取自动同步状态
bool QmlAdapter::getAutoSync() const {
    qDebug() << "Getting auto sync status";
    // 返回默认值，例如false
    return false;
}