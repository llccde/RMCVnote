#include "cloudsyncpanel_llbb.h"
#include "utils/qmladapter.h"
#include <QQmlContext>
#include <QQmlEngine>
#include <QVBoxLayout>
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

using namespace vnotex;

CloudSyncPanel::CloudSyncPanel(QWidget *p_parent)
    : QWidget(p_parent),
      m_quickView(nullptr),
      m_container(nullptr),
      m_qmlSourcePath("qrc:/qml/cloudSync.qml")
{
    setupUI();
}

void CloudSyncPanel::setupUI() {
    // 注册QmlAdapter类型到QML
    qmlRegisterType<QmlAdapter>("VNoteX", 1, 0, "CloudSyncAdapter");
    
    // 创建 QQuickView
    m_quickView = new QQuickView();
    
    // 设置 QSurfaceFormat 以实现透明背景
    QSurfaceFormat format = m_quickView->format();
    format.setAlphaBufferSize(8);  // 启用 alpha 通道
    m_quickView->setFormat(format);
    
    // 设置颜色为透明（在 Windows 上可能需要额外设置）
    m_quickView->setColor(Qt::transparent);
    
    m_quickView->setResizeMode(QQuickView::SizeRootObjectToView);
    
    // 设置 QML 源
    m_quickView->setSource(QUrl(m_qmlSourcePath));
    
    // 将 QQuickView 嵌入到 QWidget 中
    m_container = QWidget::createWindowContainer(m_quickView, this);
    m_container->setFocusPolicy(Qt::TabFocus);
    
    // 设置布局
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_container);
    
    // 设置容器大小策略
    m_container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // 连接错误信号
    connect(m_quickView, &QQuickView::statusChanged, [this](QQuickView::Status status) {
        if (status == QQuickView::Error) {
            qWarning() << "QML Errors:" << m_quickView->errors();
        } else if (status == QQuickView::Ready) {
            qDebug() << "QML loaded successfully";
        }
    });
    
#ifdef Q_OS_WIN
    // Windows 特定修复
    if (m_container->windowHandle()) {
        m_container->windowHandle()->setFlag(Qt::FramelessWindowHint, false);
    }
#endif
    
    // 确保透明背景有效
    setAttribute(Qt::WA_TranslucentBackground);
    m_container->setAttribute(Qt::WA_TranslucentBackground);
}