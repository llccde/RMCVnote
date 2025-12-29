#ifndef CLOUDSYNCPANEL_H
#define CLOUDSYNCPANEL_H

#include <QWidget>
#include <QQuickView>
#include <QWindow>

namespace vnotex {

class CloudSyncPanel : public QWidget
{
    Q_OBJECT
    
public:
    explicit CloudSyncPanel(QWidget *p_parent = nullptr);
    
private:
    void setupUI();
    
    QQuickView *m_quickView;
    QWidget *m_container;
    QString m_qmlSourcePath;
};

} // namespace vnotex

#endif // CLOUDSYNCPANEL_H