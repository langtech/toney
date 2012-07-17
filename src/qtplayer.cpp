#include "qtplayer.h"
#include <sndfile.h>

QtPlayer::QtPlayer(QObject *parent) :
    Player(parent),
    _audio_out(0),
    _af(0)
{
    registerInstance(this);
}

QtPlayer::~QtPlayer()
{
}

void QtPlayer::play(const QString &path, double start, double end)
{
    if (_audio_out)
        stop();

    _af = new AudioFile(this);

    if (_af->open(path) == false) {
        qDebug("failed to open audio file: %s: %s",
               path.toUtf8().constData(),
               _af->lastError().toUtf8().constData());
        delete _af;
        _af = 0;
        return;
    }

    _af->setRegion(start, end);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(_af->format()))
        return;

    _path = path;
    _start = start;
    _end = end;

    _audio_out = new QAudioOutput(info, _af->format(), this);

    connect(_audio_out, SIGNAL(stateChanged(QAudio::State)),
            this, SLOT(_finished_playing(QAudio::State)));

    _audio_out->start(_af);

}

void QtPlayer::hum(float *f0_samples, int n, double start, double end)
{
    qDebug() << "hum" << n << start << endl;
}

void QtPlayer::stop()
{
    qDebug() << "stop";

    if (_mutex.tryLock()) {

    if (_audio_out) {
        _audio_out->disconnect(this);
        _audio_out->stop();
        _af->close();
        qDebug() << "deleting _audio_out -- state:" << _audio_out->state();
        delete _audio_out;
        qDebug() << "ok";
        delete _af;
        _audio_out = 0;
        _af = 0;
    }

    }
}

void QtPlayer::_finished_playing(QAudio::State state)
{
    if (state == QAudio::IdleState || state == QAudio::StoppedState) {
        qDebug() << "calling stop";
        stop();
        emit finishedPlaying(_path, _start, _end);
    }
}
