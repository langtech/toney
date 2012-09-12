#ifndef ANNOTATIONSET_H
#define ANNOTATIONSET_H

#include <QSet>
#include <annotation.h>

/// AnnotationSet represents a set of annotations.
/// Each annotation is, in turn, represented by an Annotation object.
/// An AnnotationSet object can be loaded from and stored to a file. Once
/// loaded, the set is fixed. It's not possible to add a new item or delete
/// an item from the set. However, it's possible to modify information in
/// individual Annotation object.
class AnnotationSet
{
public:
    AnnotationSet();
    virtual ~AnnotationSet();
    virtual bool open(QString const& path);
    virtual bool save();
    virtual bool saveAs(QString const& path);
    const QSet<Annotation>& getAnnotations() const;
    bool modified() const;
    void resetModificationFlag();

protected:
    QString _path;
    QSet<Annotation> _anns;
};

#endif // ANNOTATIONSET_H
