#include "annotationset.h"
#include <QtXml>

AnnotationSet::AnnotationSet()
{
}

bool AnnotationSet::open(QString const& path)
{
    _path = path;
    QFile file(path);
    QDomDocument doc;
    if (!doc.setContent(&file)) {
        return false;
    }
    QDomNodeList dirs = doc.elementsByTagName("dir");
    for (int i=0; i<dirs.count(); ++i) {
        QString d = dirs.at(i).attributes().namedItem("path").nodeValue();
        QDomNodeList nodes = dirs.at(i).childNodes();
        for (int j=0; j<nodes.count(); ++j) {
            if (nodes.at(j).nodeName() == "file") {
                QDomNamedNodeMap atts = nodes.at(j).attributes();
                QString filename = atts.namedItem("name").nodeValue();
                QString label = atts.namedItem("label").nodeValue();
                QString frame_id = atts.namedItem("frmid").nodeValue();
                Annotation ann;
                ann.setAudioPath(d + "/" + filename);
                ann.setTarget(label, 0.0, 1.0);
                ann.setFrame(frame_id, 0.0, 2.0);
                _anns.insert(ann);
            }
        }
    }

    return true;
}

bool AnnotationSet::save()
{
    return saveAs(_path);
}

bool AnnotationSet::saveAs(QString const& path)
{
    Q_UNUSED(path);
    return false;
}

const QSet<Annotation> & AnnotationSet::getAnnotations() const
{
    return _anns;
}

bool AnnotationSet::modified() const
{
    foreach (const Annotation &ann, _anns) {
        if (ann.modified())
            return true;
    }
    return false;
}

void AnnotationSet::resetModificationFlag()
{
    foreach (Annotation ann, _anns)
        ann.resetModificationFlag();
}
