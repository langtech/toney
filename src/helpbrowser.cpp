#include "helpbrowser.h"
#include <QDebug>

HelpBrowser::HelpBrowser(QWidget *parent) :
    QTextBrowser(parent),
    _help_engine(0)
{
}

QVariant HelpBrowser::loadResource(int type, const QUrl &name)
{
    if (_help_engine == 0)
        return QVariant();
    if (name.scheme() == "qthelp")
        return QVariant(_help_engine->fileData(name));
    else
        return QTextBrowser::loadResource(type, name);
}

void HelpBrowser::setHelpEngine(QHelpEngine *help_engine)
{
    _help_engine = help_engine;
}
