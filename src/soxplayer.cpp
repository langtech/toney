#include "soxplayer.h"
#include <QDebug>

SoxPlayer::SoxPlayer(QObject *parent):
    Player(parent),
    _proc(this)
{
    connect(&_proc,
            SIGNAL(stateChanged(QProcess::ProcessState)),
            SLOT(_finished_playing(QProcess::ProcessState)));
}

SoxPlayer::~SoxPlayer()
{
}

void SoxPlayer::play(const QString &path, double start, double end)
{
    stop();

    _path = path;
    _start = start;
    _end = end;
    _cmd = QString("play %1 trim %2 =%3 fade 0.05 0 0.05").arg(path).arg(start).arg(end);
    _proc.start(_cmd, QIODevice::NotOpen);
}

void SoxPlayer::stop()
{
    _proc.disconnect(this);
    while (_proc.state() != QProcess::NotRunning) {
        _proc.kill();
        _proc.waitForFinished();
    }
    connect(&_proc,
            SIGNAL(stateChanged(QProcess::ProcessState)),
            SLOT(_finished_playing(QProcess::ProcessState)));
}

void SoxPlayer::_finished_playing(QProcess::ProcessState state)
{
    if (state == QProcess::NotRunning) {
        emit finishedPlaying(_path, _start, _end);
    }
}
