#ifndef CLOUDFILEMANAGERPANEL_H
#define CLOUDFILEMANAGERPANEL_H

#include <QWidget>

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
    
    ~CloudFileManagerPanel();
    
private:
    void setupUI();
    
    QQuickView *m_quickView;
    QWidget *m_container;
    QString m_qmlSourcePath;
};

} // namespace vnotex

#endif // CLOUDFILEMANAGERPANEL_H