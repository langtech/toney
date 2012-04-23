#include "filepool.h"
#include <QtXml>

FilePool::FilePool()
{
}

bool FilePool::open(QString const& path)
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
                Annotation ann;
                _files.insert(ann);
            }
        }
    }

    return true;
}

bool FilePool::save()
{
    saveAs(_path);
    return true;
}

bool FilePool::saveAs(QString const& path)
{
    return true;
}

QSet<Annotation> const& FilePool::getAnnotations() const
{
    return _files;
}

bool FilePool::setAnnotation(Annotation const& fileInfo)
{
    QSet<Annotation>::iterator i = _files.find(fileInfo);
    if (i == _files.end())
        return false;
    else {
        _files.erase(i);
        _files.insert(fileInfo);
        return true;
    }
}
