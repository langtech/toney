#include "helpbrowser.h"

HelpBrowser::HelpBrowser(QWidget *parent) :
    QTextBrowser(parent)
{
}

QVariant HelpBrowser::loadResource(int type, const QUrl &name)
{
    return QTextBrowser::loadResource(type, name);
}
