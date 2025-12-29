#ifndef __CLOUDSYNCFILEMAPPING_H__
#define __CLOUDSYNCFILEMAPPING_H__



#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QSharedPointer>

using ID = quint64;
namespace vnotex {
class CloudSyncFileMapping : public QObject
{
    Q_OBJECT

public:
    struct MappingRecord {
        ID m_nodeId;
        QString m_cid;
    };

    explicit CloudSyncFileMapping(const QString &p_databaseFile, QObject *p_parent = nullptr);
    ~CloudSyncFileMapping();

    // 打开数据库连接
    bool open();
    
    // 关闭数据库连接
    void close();
    
    // 检查数据库是否有效
    bool isValid() const { return m_valid; }
    
    // 检查是否是新建的数据库
    bool isFresh() const { return m_fresh; }
    
    // 初始化数据库表
    void initialize();
    
    // 添加映射关系
    bool addMapping(ID p_nodeId, const QString &p_cid);
    
    // 更新映射关系
    bool updateMapping(ID p_nodeId, const QString &p_cid);
    
    // 添加或更新映射关系（如果存在则更新，不存在则添加）
    bool addOrUpdateMapping(ID p_nodeId, const QString &p_cid);
    
    // 删除映射关系（根据Node ID）
    bool removeMappingByNodeId(ID p_nodeId);
    
    // 删除映射关系（根据CID）
    bool removeMappingByCid(const QString &p_cid);
    
    // 根据Node ID查询CID
    QString getCidByNodeId(ID p_nodeId);
    
    // 根据CID查询Node ID
    ID getNodeIdByCid(const QString &p_cid);
    
    // 检查Node ID是否存在映射
    bool hasMappingForNodeId(ID p_nodeId);
    
    // 检查CID是否存在映射
    bool hasMappingForCid(const QString &p_cid);
    
    // 获取所有映射记录
    QList<MappingRecord> getAllMappings();
    
    // 批量添加映射关系
    bool addMappingsBatch(const QMap<ID, QString> &p_mappings);
    
    // 清空所有映射关系
    bool clearAllMappings();

private:
    // 获取数据库连接
    QSqlDatabase getDatabase() const;
    
    // 设置数据库表结构
    void setupTables();
    
    // 查询单个映射记录
    QSharedPointer<MappingRecord> queryMappingByNodeId(ID p_nodeId);
    QSharedPointer<MappingRecord> queryMappingByCid(const QString &p_cid);

private:
    QString m_databaseFile;
    QString m_connectionName;
    bool m_valid = false;
    bool m_fresh = false;
    
    static const QString c_mappingTableName;
};
}
#endif // __CLOUDSYNCFILEMAPPING_H__