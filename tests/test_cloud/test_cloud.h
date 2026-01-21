#ifndef TEST_CLOUD_H
#define TEST_CLOUD_H

#include <QtTest>


namespace tests
{
    class TestCloud : public QObject
    {
        Q_OBJECT
    private slots:
        void initTestCase();
        void cleanupTestCase();

        // Define test cases here per slot.
        void testGetInstance();
        void testInitializeDatabase();
        void testAddAndGetFile();
        void testSnapshotVersion();
        void testGetAllFiles();
    };
}

#endif // TEST_CLOUD_H