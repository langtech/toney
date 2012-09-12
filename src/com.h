#ifndef COM_H
#define COM_H

#include <QColor>
#include "clusterbox.h"
#include "getf0paramsdialog.h"

// A communication mediator between different classes.
class Com
{
public:
    Com();
    void registerClusterBox(ClusterBox *cb);
    void registerGetF0ParamsDialog(GetF0ParamsDialog *dialog);

    QColor getColor(const QString &toneLabel);
    get_f0_session *newGetF0Session();

private:
    ClusterBox *_cb;
    GetF0ParamsDialog *_f0dialog;
};

extern Com COM;

#endif // COM_H
