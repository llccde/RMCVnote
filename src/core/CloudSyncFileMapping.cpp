#include "CloudSyncFileMapping.h"

#include <QDebug>
#include <QtSql>
namespace vnotex {
const QString CloudSyncFileMapping::c_mappingTableName = "file_mapping";

CloudSyncFileMapping::CloudSyncFileMapping(const QString &p_databaseFile, QObject *p_parent)
    : QObject(p_parent),
      m_databaseFile(p_databaseFile),
      m_connectionName("file_mapping_" + p_databaseFile)
{
}

CloudSyncFileMapping::~CloudSyncFileMapping()
{
    close();
}

bool CloudSyncFileMapping::open()
{
    // 如果已经打开，先关闭
    if (m_valid) {
        close();
    }
    
    auto db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
    db.setDatabaseName(m_databaseFile);
    
    if (!db.open()) {
        qWarning() << QStringLiteral("Failed to open file mapping database (%1) (%2)")
                          .arg(m_databaseFile, db.lastError().text());
        return false;
    }
    
    // 启用外键支持
    QSqlQuery query(db);
    if (!query.exec("PRAGMA foreign_keys = ON")) {
        qWarning() << "Failed to turn on foreign key support" << query.lastError().text();
        return false;
    }
    
    m_valid = true;
    m_fresh = db.tables().isEmpty();
    
    return true;
}

void CloudSyncFileMapping::close()
{
    if (m_valid) {
        getDatabase().close();
        QSqlDatabase::removeDatabase(m_connectionName);
        m_valid = false;
    }
}

void CloudSyncFileMapping::initialize()
{
    if (!open()) {
        return;
    }
    
    setupTables();
}

void CloudSyncFileMapping::setupTables()
{
    if (!m_valid) {
        return;
    }
    
    auto db = getDatabase();
    QSqlQuery query(db);
    
    if (m_fresh) {
        // 创建映射表
        // node_id: 节点ID，唯一
        // cid: 内容标识符，不超过64字符，唯一
        bool ret = query.exec(
            QStringLiteral(
                "CREATE TABLE %1 (\n"
                "    node_id INTEGER PRIMARY KEY,\n"
                "    cid VARCHAR(64) NOT NULL UNIQUE\n"
                ")\n")
                .arg(c_mappingTableName));
                
        if (!ret) {
            qWarning() << QStringLiteral("Failed to create database table (%1) (%2)")
                            .arg(c_mappingTableName, query.lastError().text());
            m_valid = false;
            return;
        }
        
        // 为cid创建索引以提高查询性能
        ret = query.exec(
            QStringLiteral("CREATE INDEX idx_%1_cid ON %1(cid)")
                .arg(c_mappingTableName));
                
        if (!ret) {
            qWarning() << QStringLiteral("Failed to create index on cid (%1) (%2)")
                            .arg(c_mappingTableName, query.lastError().text());
            // 索引创建失败不影响基本功能，继续执行
        }
    }
}

QSqlDatabase CloudSyncFileMapping::getDatabase() const
{
    return QSqlDatabase::database(m_connectionName);
}

bool CloudSyncFileMapping::addMapping(ID p_nodeId, const QString &p_cid)
{
    if (p_cid.isEmpty() || p_cid.length() > 64) {
        qWarning() << "CID is empty or too long (max 64 characters):" << p_cid;
        return false;
    }
    
    auto db = getDatabase();
    QSqlQuery query(db);
    
    query.prepare(
        QStringLiteral("INSERT INTO %1 (node_id, cid) VALUES (:node_id, :cid)")
            .arg(c_mappingTableName));
            
    query.bindValue(":node_id", p_nodeId);
    query.bindValue(":cid", p_cid);
    
    if (!query.exec()) {
        qWarning() << "Failed to add mapping" << query.executedQuery() 
                   << query.lastError().text();
        return false;
    }
    
    qDebug() << "Added mapping: node_id" << p_nodeId << "-> CID:" << p_cid;
    return true;
}

bool CloudSyncFileMapping::updateMapping(ID p_nodeId, const QString &p_cid)
{
    if (p_cid.isEmpty() || p_cid.length() > 64) {
        qWarning() << "CID is empty or too long (max 64 characters):" << p_cid;
        return false;
    }
    
    auto db = getDatabase();
    QSqlQuery query(db);
    
    query.prepare(
        QStringLiteral("UPDATE %1 SET cid = :cid WHERE node_id = :node_id")
            .arg(c_mappingTableName));
            
    query.bindValue(":cid", p_cid);
    query.bindValue(":node_id", p_nodeId);
    
    if (!query.exec()) {
        qWarning() << "Failed to update mapping" << query.executedQuery() 
                   << query.lastError().text();
        return false;
    }
    
    if (query.numRowsAffected() == 0) {
        qWarning() << "No mapping found for node_id:" << p_nodeId;
        return false;
    }
    
    qDebug() << "Updated mapping: node_id" << p_nodeId << "-> CID:" << p_cid;
    return true;
}

bool CloudSyncFileMapping::addOrUpdateMapping(ID p_nodeId, const QString &p_cid)
{
    if (p_cid.isEmpty() || p_cid.length() > 64) {
        qWarning() << "CID is empty or too long (max 64 characters):" << p_cid;
        return false;
    }
    
    // 先尝试更新，如果更新失败（没有对应记录），则尝试添加
    if (updateMapping(p_nodeId, p_cid)) {
        return true;
    }
    
    return addMapping(p_nodeId, p_cid);
}

bool CloudSyncFileMapping::removeMappingByNodeId(ID p_nodeId)
{
    auto db = getDatabase();
    QSqlQuery query(db);
    
    query.prepare(
        QStringLiteral("DELETE FROM %1 WHERE node_id = :node_id")
            .arg(c_mappingTableName));
            
    query.bindValue(":node_id", p_nodeId);
    
    if (!query.exec()) {
        qWarning() << "Failed to remove mapping by node_id" << query.executedQuery() 
                   << query.lastError().text();
        return false;
    }
    
    qDebug() << "Removed mapping for node_id:" << p_nodeId;
    return true;
}

bool CloudSyncFileMapping::removeMappingByCid(const QString &p_cid)
{
    auto db = getDatabase();
    QSqlQuery query(db);
    
    query.prepare(
        QStringLiteral("DELETE FROM %1 WHERE cid = :cid")
            .arg(c_mappingTableName));
            
    query.bindValue(":cid", p_cid);
    
    if (!query.exec()) {
        qWarning() << "Failed to remove mapping by cid" << query.executedQuery() 
                   << query.lastError().text();
        return false;
    }
    
    qDebug() << "Removed mapping for CID:" << p_cid;
    return true;
}

QString CloudSyncFileMapping::getCidByNodeId(ID p_nodeId)
{
    auto record = queryMappingByNodeId(p_nodeId);
    if (record) {
        return record->m_cid;
    }
    return QString();
}

ID CloudSyncFileMapping::getNodeIdByCid(const QString &p_cid)
{
    auto record = queryMappingByCid(p_cid);
    if (record) {
        return record->m_nodeId;
    }
    return 0; // 返回0表示未找到
}

bool CloudSyncFileMapping::hasMappingForNodeId(ID p_nodeId)
{
    return queryMappingByNodeId(p_nodeId) != nullptr;
}

bool CloudSyncFileMapping::hasMappingForCid(const QString &p_cid)
{
    return queryMappingByCid(p_cid) != nullptr;
}

QSharedPointer<CloudSyncFileMapping::MappingRecord> CloudSyncFileMapping::queryMappingByNodeId(ID p_nodeId)
{
    auto db = getDatabase();
    QSqlQuery query(db);
    
    query.prepare(
        QStringLiteral("SELECT node_id, cid FROM %1 WHERE node_id = :node_id")
            .arg(c_mappingTableName));
            
    query.bindValue(":node_id", p_nodeId);
    
    if (!query.exec()) {
        qWarning() << "Failed to query mapping by node_id" << query.executedQuery() 
                   << query.lastError().text();
        return nullptr;
    }
    
    if (query.next()) {
        auto record = QSharedPointer<MappingRecord>::create();
        record->m_nodeId = query.value(0).toULongLong();
        record->m_cid = query.value(1).toString();
        return record;
    }
    
    return nullptr;
}

QSharedPointer<CloudSyncFileMapping::MappingRecord> CloudSyncFileMapping::queryMappingByCid(const QString &p_cid)
{
    auto db = getDatabase();
    QSqlQuery query(db);
    
    query.prepare(
        QStringLiteral("SELECT node_id, cid FROM %1 WHERE cid = :cid")
            .arg(c_mappingTableName));
            
    query.bindValue(":cid", p_cid);
    
    if (!query.exec()) {
        qWarning() << "Failed to query mapping by cid" << query.executedQuery() 
                   << query.lastError().text();
        return nullptr;
    }
    
    if (query.next()) {
        auto record = QSharedPointer<MappingRecord>::create();
        record->m_nodeId = query.value(0).toULongLong();
        record->m_cid = query.value(1).toString();
        return record;
    }
    
    return nullptr;
}

QList<CloudSyncFileMapping::MappingRecord> CloudSyncFileMapping::getAllMappings()
{
    QList<MappingRecord> records;
    
    auto db = getDatabase();
    QSqlQuery query(db);
    
    query.prepare(
        QStringLiteral("SELECT node_id, cid FROM %1 ORDER BY node_id")
            .arg(c_mappingTableName));
    
    if (!query.exec()) {
        qWarning() << "Failed to query all mappings" << query.executedQuery() 
                   << query.lastError().text();
        return records;
    }
    
    while (query.next()) {
        MappingRecord record;
        record.m_nodeId = query.value(0).toULongLong();
        record.m_cid = query.value(1).toString();
        records.append(record);
    }
    
    return records;
}

bool CloudSyncFileMapping::addMappingsBatch(const QMap<ID, QString> &p_mappings)
{
    if (p_mappings.isEmpty()) {
        return true;
    }
    
    auto db = getDatabase();
    QSqlQuery query(db);
    
    // 开始事务
    if (!db.transaction()) {
        qWarning() << "Failed to start transaction for batch insert";
        return false;
    }
    
    query.prepare(
        QStringLiteral("INSERT OR REPLACE INTO %1 (node_id, cid) VALUES (?, ?)")
            .arg(c_mappingTableName));
    
    for (auto it = p_mappings.constBegin(); it != p_mappings.constEnd(); ++it) {
        if (it.value().isEmpty() || it.value().length() > 64) {
            qWarning() << "Invalid CID in batch:" << it.value();
            db.rollback();
            return false;
        }
        
        query.addBindValue(it.key());
        query.addBindValue(it.value());
        
        if (!query.exec()) {
            qWarning() << "Failed to add mapping in batch" << query.lastError().text();
            db.rollback();
            return false;
        }
    }
    
    if (!db.commit()) {
        qWarning() << "Failed to commit batch transaction";
        return false;
    }
    
    qDebug() << "Added" << p_mappings.size() << "mappings in batch";
    return true;
}

bool CloudSyncFileMapping::clearAllMappings()
{
    auto db = getDatabase();
    QSqlQuery query(db);
    
    query.prepare(QStringLiteral("DELETE FROM %1").arg(c_mappingTableName));
    
    if (!query.exec()) {
        qWarning() << "Failed to clear all mappings" << query.executedQuery() 
                   << query.lastError().text();
        return false;
    }
    
    qDebug() << "Cleared all mappings";
    return true;
}
}