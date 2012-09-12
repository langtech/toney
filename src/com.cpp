#include "com.h"

Com::Com():
    _cb(0),
    _f0dialog(0)
{
}

void Com::registerClusterBox(ClusterBox *cb)
{
    _cb = cb;
}

void Com::registerGetF0ParamsDialog(GetF0ParamsDialog *dialog)
{
    _f0dialog = dialog;
}

QColor Com::getColor(const QString &toneLabel)
{
    if (_cb) {
        Cluster *c = _cb->getCluster(toneLabel);
        if (c)
            return c->getColor();
    }
    return Qt::transparent;
}

get_f0_session *Com::newGetF0Session()
{
    if (_f0dialog)
        return _f0dialog->newGetF0Session();
    else
        return 0;
}

Com COM;
