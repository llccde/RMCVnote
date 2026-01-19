// cloudfilemanagerpanel.h
#ifndef CLOUDFILEMANAGERPANEL_H
#define CLOUDFILEMANAGERPANEL_H

#include <QWidget>
#include <memory>

QT_BEGIN_NAMESPACE
class QQuickView;
class QWidget;
QT_END_NAMESPACE

namespace vnotex {

class CloudFileManagerPanel : public QWidget
{
    Q_OBJECT
    
public:
    explicit CloudFileManagerPanel(QWidget *p_parent = nullptr);
    
    // 添加尺寸调整事件
    void resizeEvent(QResizeEvent *event) override;
    
private:
    void setupUI();
    
    QQuickView *m_quickView;
    QWidget *m_container;
    QString m_qmlSourcePath;
    
    bool m_cleanedUp = false; // 防止重复清理
};

} // namespace vnotex

#endif // CLOUDFILEMANAGERPANEL_H