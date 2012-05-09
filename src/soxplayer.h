#ifndef SOXPLAYER_H
#define SOXPLAYER_H

#include <QProcess>
#include <QString>
#include <player.h>

class SoxPlayer: public Player
{
    Q_OBJECT

public:
    explicit SoxPlayer(QObject *parent = 0);
    virtual ~SoxPlayer();

public slots:
    void play(const QString &path, double start, double end);
    void stop();

private slots:
    void _finished_playing(QProcess::ProcessState);

private:
    void _find_play_command();

private:
    QString _cmd;
    QProcess _proc;
    QString _path;
    QString _play; // play command
    double _start;
    double _end;

};

#endif // SOXPLAYER_H
