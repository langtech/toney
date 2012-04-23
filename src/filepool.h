#ifndef FILEPOOL_H
#define FILEPOOL_H

#include <QSet>
#include <annotation.h>

/// FilePool represents a set of files.
/// Each of the files is, in turn, represented by a FileInfo object.
/// A FilePool object can be loaded from and stored to an XML file.Once loaded,
/// the pool is fixed. It's not possible to add a new item or delete an item
/// from the pool. However, it's possible to modify information about
/// individual files.
class FilePool
{
public:
    FilePool();
    virtual bool open(QString const& path);
    virtual bool save();
    virtual bool saveAs(QString const& path);
    QSet<Annotation> const& getAnnotations() const;
    bool setAnnotation(Annotation const& ann);

private:
    QString _path;
    QSet<Annotation> _files;
};

#endif // FILEPOOL_H
