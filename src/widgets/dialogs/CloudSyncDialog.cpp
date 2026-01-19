#include "CloudSyncDialog.h"
#include "utils/CloudFileNetWork.h"
#include <QIcon>
#include <QFileInfo>
#include <QCheckBox>

// 实现 CloudSyncDialog 内部类的构造函数和方法

// CloudFileInfo 构造函数
CloudSyncDialog::CloudFileInfo::CloudFileInfo(const QString& name, const QString& id,
                                              qint64 size, const QDateTime& lastModified)
    : name(name), id(id), size(size), lastModified(lastModified) {}

// SubmitConfig 构造函数
CloudSyncDialog::SubmitConfig::SubmitConfig() : mode(CloudSyncDialog::OperationMode::SelectExisting), overwrite(false) {}

// SubmitConfig::getTargetIdentifier 方法
QString CloudSyncDialog::SubmitConfig::getTargetIdentifier() const {
    return mode == CloudSyncDialog::OperationMode::SelectExisting ? selectedId : newFileName;
}

// SubmitConfig::getDescription 方法
QString CloudSyncDialog::SubmitConfig::getDescription() const {
    if (mode == CloudSyncDialog::OperationMode::SelectExisting) {
        return QString("选择已有文件 (ID: %1)").arg(selectedId);
    } else {
        return QString("创建新文件: %1").arg(newFileName);
    }
}

CloudSyncDialog::CloudSyncDialog(QWidget *parent,
                                 SubmitCallback submitCallback,
                                 const QString& localFilePath)
    : QDialog(parent)
    , m_submitCallback(submitCallback)
    , m_localFilePath(localFilePath)
{
    m_currentConfig.mode = OperationMode::SelectExisting;
    m_currentConfig.overwrite = false;
    m_currentConfig.localFilePath = localFilePath;

    setWindowTitle(tr("同步到云端"));
    setMinimumSize(500, 400);

    setupUI();

    // Load cloud files internally
    loadCloudFiles();

    // 如果有本地文件路径，显示文件信息
    if (!localFilePath.isEmpty()) {
        QFileInfo fileInfo(localFilePath);
        m_localFileLabel->setText(tr("本地文件: %1 ").arg(fileInfo.fileName()));
    }
}

// 辅助函数：格式化文件大小
QString formatFileSize(qint64 bytes) {
    const qint64 KB = 1024;
    const qint64 MB = KB * 1024;
    const qint64 GB = MB * 1024;
    
    if (bytes >= GB) {
        return QString("%1 GB").arg(QString::number(bytes / (double)GB, 'f', 2));
    } else if (bytes >= MB) {
        return QString("%1 MB").arg(QString::number(bytes / (double)MB, 'f', 2));
    } else if (bytes >= KB) {
        return QString("%1 KB").arg(QString::number(bytes / (double)KB, 'f', 2));
    } else {
        return QString("%1 字节").arg(bytes);
    }
}

void CloudSyncDialog::setExistingFiles(const QVector<CloudFileInfo>& files)
{
    m_existingFiles = files;
    m_fileListWidget->clear();

    if (files.isEmpty()) {
        m_noFilesLabel->show();
        m_fileListWidget->hide();
        m_selectedFileInfo->hide();
    } else {
        m_noFilesLabel->hide();
        m_fileListWidget->show();
        m_selectedFileInfo->show();

        for (const auto& file : files) {
            QListWidgetItem* item = new QListWidgetItem(file.name, m_fileListWidget);
            item->setData(Qt::UserRole, file.id);
            // 添加额外的文件信息作为tooltip
            QString tooltip = tr("名称: %1\n大小: %2\n修改时间: %3")
                .arg(file.name)
                .arg(formatFileSize(file.size))
                .arg(file.lastModified.toString("yyyy-MM-dd hh:mm:ss"));
            item->setToolTip(tooltip);
        }

        // 默认选择第一项
        if (m_fileListWidget->count() > 0) {
            m_fileListWidget->setCurrentRow(0);
            updateFileInfoDisplay();
        }
    }
}

void CloudSyncDialog::setSubmitCallback(SubmitCallback callback)
{
    m_submitCallback = callback;
}

void CloudSyncDialog::setLocalFilePath(const QString& filePath)
{
    m_localFilePath = filePath;
    m_currentConfig.localFilePath = filePath;
    
    if (!filePath.isEmpty()) {
        QFileInfo fileInfo(filePath);
        m_localFileLabel->setText(tr("本地文件: %1 (%2)").arg(fileInfo.fileName())
                                  .arg(formatFileSize(fileInfo.size())));
    }
}

void CloudSyncDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // 本地文件信息
    m_localFileLabel = new QLabel();
    m_localFileLabel->setStyleSheet("font-weight: bold; padding: 5px;");
    mainLayout->addWidget(m_localFileLabel);
    
    // 分隔线
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(line);
    
    // 错误显示标签
    m_errorLabel = new QLabel();
    m_errorLabel->setStyleSheet("color: red; background-color: #FFF0F0; padding: 5px; border-radius: 3px;");
    m_errorLabel->setWordWrap(true);
    m_errorLabel->hide();
    mainLayout->addWidget(m_errorLabel);
    
    // 选项卡
    m_tabWidget = new QTabWidget();
    
    // 选择已有文件选项卡
    m_existingTab = new QWidget();
    QVBoxLayout* existingLayout = new QVBoxLayout(m_existingTab);
    
    QLabel* existingHeader = new QLabel(tr("请选择要同步到的云端文件:"));
    existingHeader->setStyleSheet("font-weight: bold;");
    
    m_fileListWidget = new QListWidget();
    m_fileListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    
    m_noFilesLabel = new QLabel(tr("没有找到云端文件"));
    m_noFilesLabel->setAlignment(Qt::AlignCenter);
    
    m_selectedFileInfo = new QLabel();
    m_selectedFileInfo->setStyleSheet("background-color: #F0F8FF; padding: 5px; border-radius: 3px;");
    m_selectedFileInfo->setWordWrap(true);
    
    existingLayout->addWidget(existingHeader);
    existingLayout->addWidget(m_fileListWidget);
    existingLayout->addWidget(m_noFilesLabel);
    existingLayout->addWidget(m_selectedFileInfo);
    
    m_tabWidget->addTab(m_existingTab, tr("选择已有文件"));
    
    // 创建新文件选项卡
    m_newTab = new QWidget();
    QVBoxLayout* newLayout = new QVBoxLayout(m_newTab);
    
    QLabel* newFileLabel = new QLabel(tr("请输入新文件名:"));
    m_newFileEdit = new QLineEdit();
    m_newFileEdit->setPlaceholderText(tr("例如: document.txt"));
    
    m_overwriteCheckbox = new QCheckBox(tr("覆盖同名文件（如果存在）"));
    
    m_newFileErrorLabel = new QLabel();
    m_newFileErrorLabel->setStyleSheet("color: red;");
    m_newFileErrorLabel->hide();
    
    newLayout->addWidget(newFileLabel);
    newLayout->addWidget(m_newFileEdit);
    newLayout->addWidget(m_overwriteCheckbox);
    newLayout->addWidget(m_newFileErrorLabel);
    newLayout->addStretch();
    
    m_tabWidget->addTab(m_newTab, tr("创建新文件"));
    
    mainLayout->addWidget(m_tabWidget);
    
    // 按钮布局
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    m_submitButton = new QPushButton(tr("确定"));
    m_submitButton->setDefault(true);
    m_cancelButton = new QPushButton(tr("取消"));
    
    m_submitButton->setMinimumWidth(100);
    m_cancelButton->setMinimumWidth(100);
    
    buttonLayout->addWidget(m_submitButton);
    buttonLayout->addWidget(m_cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // 连接信号槽
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &CloudSyncDialog::onTabChanged);
    connect(m_fileListWidget, &QListWidget::currentItemChanged, 
            this, [this]() { updateFileInfoDisplay(); });
    connect(m_submitButton, &QPushButton::clicked, this, &CloudSyncDialog::onSubmit);
    connect(m_cancelButton, &QPushButton::clicked, this, &CloudSyncDialog::onCancel);
    connect(m_newFileEdit, &QLineEdit::textChanged, this, [this]() {
        clearError();
        m_newFileErrorLabel->hide();
    });
    
    // 初始化状态
    onTabChanged(0);
}

void CloudSyncDialog::updateFileInfoDisplay()
{
    QListWidgetItem* currentItem = m_fileListWidget->currentItem();
    if (currentItem) {
        QString fileId = currentItem->data(Qt::UserRole).toString();
        
        // 从现有文件中查找详细信息
        for (const auto& file : m_existingFiles) {
            if (file.id == fileId) {
                QString info = tr("选中的文件:\n"
                                "名称: %1\n"
                                "大小: %2\n"
                                "最后修改: %3")
                    .arg(file.name)
                    .arg(formatFileSize(file.size))
                    .arg(file.lastModified.toString("yyyy-MM-dd hh:mm:ss"));
                m_selectedFileInfo->setText(info);
                break;
            }
        }
    } else {
        m_selectedFileInfo->setText(tr("未选择文件"));
    }
}

void CloudSyncDialog::onTabChanged(int index)
{
    m_currentConfig.mode = (index == 0) ? OperationMode::SelectExisting : OperationMode::CreateNew;
    clearError();
}

void CloudSyncDialog::onSubmit()
{
    clearError();
    
    // 准备配置
    if (m_currentConfig.mode == OperationMode::SelectExisting) {
        QListWidgetItem* currentItem = m_fileListWidget->currentItem();
        if (!currentItem) {
            showError(tr("请选择一个云端文件"));
            return;
        }
        m_currentConfig.selectedId = currentItem->data(Qt::UserRole).toString();
        m_currentConfig.newFileName.clear();
    } else {
        m_currentConfig.newFileName = m_newFileEdit->text().trimmed();
        if (m_currentConfig.newFileName.isEmpty()) {
            m_newFileErrorLabel->setText(tr("文件名不能为空"));
            m_newFileErrorLabel->show();
            return;
        }
        m_currentConfig.selectedId.clear();
        m_currentConfig.overwrite = m_overwriteCheckbox->isChecked();
    }
    
    if (!m_submitCallback) {
        showError(tr("提交回调函数未设置"));
        return;
    }
    
    // 调用回调函数，传递完整的配置结构体
    QString error = m_submitCallback(m_currentConfig);
    
    if (error.isEmpty()) {
        accept(); // 成功，关闭对话框
    } else {
        showError(error);
    }
}

void CloudSyncDialog::onCancel()
{
    reject();
}

void CloudSyncDialog::clearError()
{
    m_errorLabel->clear();
    m_errorLabel->hide();
}

void CloudSyncDialog::showError(const QString& error)
{
    m_errorLabel->setText(error);
    m_errorLabel->show();

    // 根据当前模式定位到相应的选项卡
    if (m_currentConfig.mode == OperationMode::CreateNew) {
        m_tabWidget->setCurrentIndex(1);
        m_newFileEdit->setFocus();
    } else {
        m_tabWidget->setCurrentIndex(0);
        m_fileListWidget->setFocus();
    }
}

void CloudSyncDialog::loadCloudFiles()
{
    // Load cloud files internally using CloudFileNetWork
    auto backend = CloudFileNetWork::getInstance();
    auto existCloudFile = backend->getAllFiles();
    QVector<CloudFileInfo> files;
    if(existCloudFile.isSuccess()){
        for(auto i:*existCloudFile.data){
            files.push_back(CloudFileInfo(
                i.fileName,
                CloudFileNetWork::IDToString(i.fileID),
                0,  // size - could be calculated if needed
                i.updateTime
            ));
        }
    }
    setExistingFiles(files);
}