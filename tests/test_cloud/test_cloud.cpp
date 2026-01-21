#include "test_cloud.h"
#include <QTemporaryDir>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include "CloudFileNetWork.h"
using namespace tests;

void TestCloud::initTestCase()
{
    qDebug() << "Initializing CloudFileNetWork test case";
    
    // 设置测试数据库路径
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    
    // 获取CloudFileNetWork实例
    auto *instance = CloudFileNetWork::getInstance();
    Q_UNUSED(instance);
    
    // 初始化数据库
    auto result = CloudFileNetWork::getInstance()->initializeDatabase();
    QVERIFY(result.isSuccess());
}

void TestCloud::cleanupTestCase()
{
    qDebug() << "Cleaning up CloudFileNetWork test case";
    
    // 移除测试数据库连接
    QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
}

void TestCloud::testGetInstance()
{
    auto *instance1 = CloudFileNetWork::getInstance();
    auto *instance2 = CloudFileNetWork::getInstance();
    
    QVERIFY(instance1 != nullptr);
    QCOMPARE(instance1, instance2);
}

void TestCloud::testInitializeDatabase()
{
    auto result = CloudFileNetWork::getInstance()->initializeDatabase();
    QVERIFY(result.isSuccess());
}

void TestCloud::testAddAndGetFile()
{
    QString fileName = "test_file.txt";
    QString content = "This is a test file content.";
    QString description = "Test file for unit testing";
    
    // 添加文件
    auto addResult = CloudFileNetWork::getInstance()->addFile(fileName, content, description);
    QVERIFY(addResult.isSuccess());
    
    auto fileID = *(addResult.data);
    QVERIFY(fileID > 0);
    
    // 获取文件信息
    auto getResult = CloudFileNetWork::getInstance()->getFileByFileID(fileID);
    QVERIFY(getResult.isSuccess());
    
    auto fileInfo = *(getResult.data);
    QCOMPARE(fileInfo.fileName, fileName);
}

void TestCloud::testSnapshotVersion()
{
    QString fileName = "version_test_file.txt";
    QString initialContent = "Initial content";
    QString description = "Test file for versioning";

    // 添加文件
    auto addResult = CloudFileNetWork::getInstance()->addFile(fileName, initialContent, description);
    QVERIFY(addResult.isSuccess());

    auto fileID = *(addResult.data);
    QVERIFY(fileID > 0);

    // 获取初始版本ID
    auto initialVersionResult = CloudFileNetWork::getInstance()->getLatestVersionID(fileID);
    QVERIFY(initialVersionResult.isSuccess());
    auto initialVersionID = *(initialVersionResult.data);
    QVERIFY(initialVersionID > 0);

    // 更新内容并创建新版本
    QString updatedContent = "Updated content";
    auto updateResult = CloudFileNetWork::getInstance()->updateFileContent(fileID, updatedContent);
    QVERIFY(updateResult.isSuccess());

    // 创建快照版本
    QString versionDescription = "Updated version";
    auto snapshotResult = CloudFileNetWork::getInstance()->snapshotVersionForFile(fileID, versionDescription);
    QVERIFY(snapshotResult.isSuccess());

    auto newVersionID = *(snapshotResult.data);
    QVERIFY(newVersionID > initialVersionID);

    // 获取最新版本ID
    auto latestVersionResult = CloudFileNetWork::getInstance()->getLatestVersionID(fileID);
    QVERIFY(latestVersionResult.isSuccess());
    auto latestVersionID = *(latestVersionResult.data);
    QCOMPARE(latestVersionID, newVersionID);

    // 获取版本内容
    auto contentResult = CloudFileNetWork::getInstance()->getVersionContent(newVersionID);
    QVERIFY(contentResult.isSuccess());
    QCOMPARE(*(contentResult.data), updatedContent);
}

void TestCloud::testGetAllFiles()
{
    // 先添加几个测试文件
    QStringList fileNames = {"test1.txt", "test2.txt", "test3.txt"};
    QString content = "Test content";

    QList<CloudFileNetWorkFileAndVersionID> fileIDs;
    for(const auto &fileName : fileNames) {
        auto result = CloudFileNetWork::getInstance()->addFile(fileName, content, "Test file");
        QVERIFY(result.isSuccess());
        fileIDs.append(*(result.data));
    }

    // 获取所有文件
    auto allFilesResult = CloudFileNetWork::getInstance()->getAllFiles(SortType::SORT_BY_CREATE_TIME);
    QVERIFY(allFilesResult.isSuccess());

    auto files = *(allFilesResult.data);

    // 检查是否包含我们添加的文件
    QVERIFY(files.size() >= fileNames.size()); // 可能包含示例数据

    // 检查特定文件是否存在
    QStringList retrievedFileNames;
    for(const auto &file : files) {
        retrievedFileNames << file.fileName;
    }

    for(const auto &fileName : fileNames) {
        QVERIFY(retrievedFileNames.contains(fileName));
    }
}

QTEST_MAIN(tests::TestCloud)