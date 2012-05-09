#include "configfile.h"
#include <QDir>
#include <QFile>
#include <QDebug>

ConfigFile::ConfigFile()
{
    _error.clear();
    if (QDir::home().exists(".toney")) {
        _init(QDir::homePath() + QDir::separator() + ".toney");
    }
    else {
        _error = QString("Default config file %1 doesn't exist.")
                .arg(QDir::homePath() + QDir::separator() + ".toney");
    }
}

ConfigFile::ConfigFile(const QString &path)
{
    _error.clear();
    _init(path);
}

QString ConfigFile::get(const QString &name)
{
    if (_data.contains(name)) {
        return _data[name];
    }
    else {
        return "";
    }
}

QStringList ConfigFile::getAsList(const QString &name)
{
    if (_data.contains(name)) {
        return _data[name].split(':');
    }
    else {
        return QStringList();
    }
}

QString ConfigFile::error() const
{
    return _error;
}

void ConfigFile::refresh()
{
    _error.clear();
    _init(_path);
}

void ConfigFile::_init(const QString &path)
{
    QFile file(path);
    // this instance is useless if path doesn't exist
    if (file.exists()) {
        if (file.open(QIODevice::ReadOnly)) {
            _path = path;
        }
        else {
            _error = QString("File not readable: %1").arg(path);
            return;
        }
    }
    else {
        _error = QString("File not found: %1").arg(path);
        return;
    }

    QTextStream text(&file);
    QString line = text.readLine();
    while (!line.isNull()) {
        if (!line.startsWith('#')) {
            int idx = line.indexOf('\t');
            QString name(line.left(idx).trimmed());
            QString value(line.mid(idx+1).trimmed());
            _data[name] = value;
        }
        line = text.readLine();
    }
}
