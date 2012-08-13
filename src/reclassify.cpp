#include "annotation.h"
#include "toney.h"

void reclassify(QHash<const Annotation,QString> &s)
{
    QHash<const Annotation,QString>::iterator i = s.begin();
    for (; i != s.end(); ++i) {
        const Annotation ann = i.key();
        QString cluster = i.value();
        qDebug() << ann.getTargetLabel() << cluster;
    }
}
