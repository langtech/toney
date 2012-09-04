#include "com.h"

Com::Com():
    _cb(0)
{
}

void Com::registerClusterBox(ClusterBox *cb)
{
    _cb = cb;
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

Com COM;
