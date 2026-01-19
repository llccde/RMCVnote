#ifndef CLOUDSYNCDIALOG_H
#define CLOUDSYNCDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QMap>
#include <QDateTime>

class CloudSyncDialog : public QDialog
{
    Q_OBJECT

public:
    // 操作模式枚举
    enum class OperationMode {
        SelectExisting,
        CreateNew
    };

    // 云端文件信息结构
    struct CloudFileInfo {
        QString name;
        QString id;
        qint64 size;           // 文件大小
        QDateTime lastModified; // 最后修改时间

        CloudFileInfo(const QString& name = "", const QString& id = "",
                      qint64 size = 0, const QDateTime& lastModified = QDateTime());
    };

    // 提交配置结构体
    struct SubmitConfig {
        OperationMode mode;
        QString selectedId;      // 当选择已有文件时使用
        QString newFileName;     // 当创建新文件时使用
        QString localFilePath;   // 本地文件路径（如果需要）
        bool overwrite;          // 是否覆盖现有文件

        SubmitConfig();

        // 获取实际要使用的文件名或ID
        QString getTargetIdentifier() const;

        // 获取描述信息
        QString getDescription() const;
    };

    using SubmitCallback = std::function<QString(const SubmitConfig&)>;

    explicit CloudSyncDialog(QWidget *parent = nullptr,
                             SubmitCallback submitCallback = nullptr,
                             const QString& localFilePath = "");
    
    void setExistingFiles(const QVector<CloudFileInfo>& files);
    void setSubmitCallback(SubmitCallback callback);
    void setLocalFilePath(const QString& filePath);
    
private slots:
    void onTabChanged(int index);
    void onSubmit();
    void onCancel();
    
private:
    void setupUI();
    void clearError();
    void showError(const QString& error);
    void updateFileInfoDisplay();
    void loadCloudFiles();
    
    // 状态
    SubmitConfig m_currentConfig;
    QVector<CloudFileInfo> m_existingFiles;
    SubmitCallback m_submitCallback;
    QString m_localFilePath;
    
    // UI组件
    QTabWidget* m_tabWidget;
    
    // 选择已有文件选项卡
    QWidget* m_existingTab;
    QListWidget* m_fileListWidget;
    QLabel* m_noFilesLabel;
    QLabel* m_selectedFileInfo;
    
    // 创建新文件选项卡
    QWidget* m_newTab;
    QLineEdit* m_newFileEdit;
    QCheckBox* m_overwriteCheckbox;
    QLabel* m_newFileErrorLabel;
    
    // 公共按钮
    QPushButton* m_submitButton;
    QPushButton* m_cancelButton;
    
    // 错误显示
    QLabel* m_errorLabel;
    
    // 本地文件信息
    QLabel* m_localFileLabel;
};

#endif // CLOUDSYNCDIALOG_H