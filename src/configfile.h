#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <QString>
#include <QStringList>
#include <QHash>

/// Read and parse config file
class ConfigFile
{
public:
    ConfigFile();
    ConfigFile(const QString &path);
    QString get(const QString &name);
    // split value using colon (:)
    QStringList getAsList(const QString &name);
    // empty string means no error
    QString error() const;
    void refresh();

private:
    void _init(const QString &path);

private:
    QString _path;
    QHash<QString,QString> _data;
    QString _error;
};

#endif // CONFIGFILE_H
