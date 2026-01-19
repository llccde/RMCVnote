#include <QMap>
#include <QString>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFile>
#include <qcontainerfwd.h>
#include <qdir.h>
#include <qlist.h>
#include <qobject.h>
#include "QSharedPointer"
#include "qstandardpaths.h"
#include "global.h"
template <typename Key, typename Value>
class LocalDBMap : public QMap<Key, Value> {
private:
    QString m_dbPath;
    QString m_tableName;
    QSqlDatabase m_db;

public:
    // 构造函数
    inline LocalDBMap(const QString& dbPath = "localdbmap.db", const QString& table = "local_map")
        : m_dbPath(dbPath), m_tableName(table) {

        // 初始化数据库
        if (!initDatabase()) {
            qWarning() << "数据库初始化失败";
            return;
        }

        // 创建表
        if (!createTable()) {
            qWarning() << "表创建失败";
            return;
        }

        // 从数据库加载数据
        loadFromDB();
    }

    // 析构函数
    inline ~LocalDBMap() {
        if (m_db.isOpen()) {
            m_db.close();
        }
    }
    inline QList<Key> getKeyByValue(Value v){
        QList<Key> keys;
        for (auto it = this->constBegin(); it != this->constEnd(); ++it) {
            if (it.value() == v) {
                keys.append(it.key());
            }
        }
        return keys;
    }
    // 从数据库加载数据到map
    inline bool loadFromDB() {
        // 清空当前map
        this->clear();

        QSqlQuery query(m_db);
        QString sql = QString("SELECT key, value FROM %1").arg(m_tableName);

        if (!query.exec(sql)) {
            qWarning() << "查询数据失败:" << query.lastError().text();
            return false;
        }

        while (query.next()) {
            QString keyStr = query.value(0).toString();
            QString valueStr = query.value(1).toString();

            Key key = deserializeKey(keyStr);
            Value value = deserializeValue(valueStr);

            this->insert(key, value);
        }

        qDebug() << "从数据库加载了" << this->size() << "条记录";
        return true;
    }

    // 保存map到数据库
    inline bool saveToLocalDB() {
        // 开始事务
        m_db.transaction();

        // 清空表
        QSqlQuery clearQuery(m_db);
        QString clearSql = QString("DELETE FROM %1").arg(m_tableName);
        if (!clearQuery.exec(clearSql)) {
            m_db.rollback();
            qWarning() << "清空表失败:" << clearQuery.lastError().text();
            return false;
        }

        // 插入数据
        QSqlQuery insertQuery(m_db);
        QString insertSql = QString("INSERT INTO %1 (key, value) VALUES (?, ?)").arg(m_tableName);
        insertQuery.prepare(insertSql);

        for (auto it = this->constBegin(); it != this->constEnd(); ++it) {
            QString keyStr = serializeKey(it.key());
            QString valueStr = serializeValue(it.value());

            insertQuery.addBindValue(keyStr);
            insertQuery.addBindValue(valueStr);

            if (!insertQuery.exec()) {
                m_db.rollback();
                qWarning() << "插入数据失败:" << insertQuery.lastError().text();
                return false;
            }
        }

        // 提交事务
        if (!m_db.commit()) {
            qWarning() << "提交事务失败:" << m_db.lastError().text();
            return false;
        }

        qDebug() << "保存了" << this->size() << "条记录到数据库";
        return true;
    }

    // 获取数据库路径
    inline QString getDBPath() const {
        return m_dbPath;
    }

    // 获取表名
    inline QString getTableName() const {
        return m_tableName;
    }

    // 重新设置数据库和表（会重新加载数据）
    inline bool resetDB(const QString& dbPath, const QString& table) {
        m_dbPath = dbPath;
        m_tableName = table;

        // 重新初始化数据库
        if (!initDatabase()) {
            return false;
        }

        // 创建新表
        if (!createTable()) {
            return false;
        }

        // 从新数据库加载数据
        return loadFromDB();
    }

    // 清除数据库表
    inline bool clearDB() {
        QSqlQuery query(m_db);
        QString sql = QString("DELETE FROM %1").arg(m_tableName);

        if (!query.exec(sql)) {
            qWarning() << "清除表失败:" << query.lastError().text();
            return false;
        }

        this->clear();
        qDebug() << "数据库表已清空";
        return true;
    }

    // 删除数据库文件
    inline bool removeDBFile() {
        if (m_db.isOpen()) {
            m_db.close();
        }

        QString connectionName = m_db.connectionName();
        QSqlDatabase::removeDatabase(connectionName);

        return QFile::remove(m_dbPath);
    }

private:
    // 初始化数据库连接
    inline bool initDatabase() {
        // 如果数据库已连接，则关闭重新连接
        if (m_db.isOpen()) {
            m_db.close();
        }

        // 设置数据库连接
        m_db = QSqlDatabase::addDatabase("QSQLITE", QString("LocalDBMap_%1").arg((quintptr)this));
        m_db.setDatabaseName(m_dbPath);

        // 打开数据库
        if (!m_db.open()) {
            qWarning() << "无法打开数据库:" << m_db.lastError().text();
            return false;
        }

        return true;
    }

    // 创建表
    inline bool createTable() {
        QSqlQuery query(m_db);

        // SQL语句：创建表，使用通用的TEXT类型存储键值
        QString sql = QString("CREATE TABLE IF NOT EXISTS %1 ("
                              "key TEXT PRIMARY KEY, "
                              "value TEXT)").arg(m_tableName);

        if (!query.exec(sql)) {
            qWarning() << "创建表失败:" << query.lastError().text();
            return false;
        }

        return true;
    }

    // 序列化值到字符串
    inline QString serializeValue(const Value& value) const {
        // 使用QVariant进行序列化
        QVariant variant = QVariant::fromValue(value);
        if (!variant.isValid()) {
            qWarning() << "无法序列化值，类型可能未注册";
            return QString();
        }
        return variant.toString();
    }

    // 反序列化字符串到值
    inline Value deserializeValue(const QString& str) const {
        QVariant variant(str);
        return variant.value<Value>();
    }

    // 序列化键到字符串
    inline QString serializeKey(const Key& key) const {
        QVariant variant = QVariant::fromValue(key);
        if (!variant.isValid()) {
            qWarning() << "无法序列化键，类型可能未注册";
            return QString();
        }
        return variant.toString();
    }

    // 反序列化字符串到键
    inline Key deserializeKey(const QString& str) const {
        QVariant variant(str);
        return variant.value<Key>();
    }
};
struct NoteRecord{
    vnotex::ID book;
    vnotex::ID note;

    // 为了用作QMap的键，需要定义operator<
    inline bool operator<(const NoteRecord& other) const {
        if (book != other.book) {
            return book < other.book;
        }
        return note < other.note;
    }

    // 定义相等运算符
    inline bool operator==(const NoteRecord& other) const {
        return book == other.book && note == other.note;
    }
};
Q_DECLARE_METATYPE(NoteRecord);

class CloudLocalMapping:public LocalDBMap<NoteRecord,QString>{
    public:
    
    inline explicit CloudLocalMapping(QString user = "default"):LocalDBMap<NoteRecord,QString>(
        QDir(QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation
        )).filePath("FileMapping.db"),
        QString("%1_table").arg(user)
    ){
        
    }  
    bool inline has(NoteRecord note){
        return find(note)!=end();
    }
};