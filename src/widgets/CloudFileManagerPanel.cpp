// cloudfilemanagerpanel.cpp
#include "cloudfilemanagerpanel.h"
#include "utils/cloudfilemanageradapter.h"
#include <QQmlContext>
#include <QQmlEngine>
#include <QVBoxLayout>
#include <QDebug>
#include <QQuickView>
#include <QWindow>
#include <QQuickStyle>
#include <QResizeEvent>
#ifdef Q_OS_WIN
#include <windows.h>
#endif

using namespace vnotex;

CloudFileManagerPanel::CloudFileManagerPanel(QWidget *p_parent)
    : QWidget(p_parent),
      m_quickView(nullptr),
      m_container(nullptr),
      m_qmlSourcePath("qrc:/qml/CloudFileManager/main.qml")
{
    setupUI();
}



void CloudFileManagerPanel::setupUI() {
    // 注册QmlAdapter类型到QML
    qmlRegisterSingletonInstance<CloudFileManagerAdapter>(
    "VNoteX", 1, 0, 
    "CloudFileManagerAdapter", CloudFileManagerAdapter::getAdapter());
    // 创建 QQuickView
    m_quickView = new QQuickView();
    
    // 设置 QSurfaceFormat 以实现透明背景
    QSurfaceFormat format = m_quickView->format();
    format.setAlphaBufferSize(8);
    m_quickView->setFormat(format);
    
    // 设置颜色为透明
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
            qDebug() << "CloudFileManagerPanel QML loaded successfully";
        }
    });
    
    // 连接销毁信号
    connect(m_quickView, &QQuickView::destroyed, [this]() {
        qDebug() << "QQuickView destroyed";
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

void CloudFileManagerPanel::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    if (m_container && m_quickView) {
        m_container->resize(event->size());
    }
}