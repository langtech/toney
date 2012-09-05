#ifndef COM_H
#define COM_H

#include <QColor>
#include "clusterbox.h"

// A communication mediator between different classes.
class Com
{
public:
    Com();
    void registerClusterBox(ClusterBox *cb);

    QColor getColor(const QString &toneLabel);

private:
    ClusterBox *_cb;
};

extern Com COM;

#endif // COM_H
