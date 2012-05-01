#ifndef HELPBROWSER_H
#define HELPBROWSER_H

#include <QTextBrowser>

class HelpBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    explicit HelpBrowser(QWidget *parent = 0);
    QVariant loadResource(int type, const QUrl &name);

signals:
    
public slots:
    
};

#endif // HELPBROWSER_H
