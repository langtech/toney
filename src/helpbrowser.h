#ifndef HELPBROWSER_H
#define HELPBROWSER_H

#include <QTextBrowser>
#include <QtHelp>

class HelpBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    explicit HelpBrowser(QWidget *parent = 0);
    QVariant loadResource(int type, const QUrl &name);
    void setHelpEngine(QHelpEngine *help_engine);

signals:
    
public slots:

private:
    QHelpEngine *_help_engine;
};

#endif // HELPBROWSER_H
