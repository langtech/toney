#include "qtplayer.h"
#include <sndfile.h>

QtPlayer::QtPlayer(QObject *parent) :
    Player(parent),
    _audio_out(0),
    _af(0),
    _wav(0)
{
    registerInstance(this);
}

QtPlayer::~QtPlayer()
{
}

void QtPlayer::play(const QString &path, double start, double end)
{
    QMutexLocker lock(&_mutex);

    qDebug() << "Play" << path << start << end;

    if (_audio_out) {
        qDebug() << "Still playing -- force stop";
        _stop();
    }

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

    qDebug() << "Started playing";
}

void QtPlayer::hum(float *f0_samples, int nsamp, double start, double end)
{
    _wav = new SineWaveFile(f0_samples, nsamp, end - start, this);
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(_wav->format())) {
        qDebug() << "audio format not supported";
        return;
    }

    _audio_out = new QAudioOutput(info, _wav->format(), this);

    connect(_audio_out, SIGNAL(stateChanged(QAudio::State)),
            this, SLOT(_finished_humming(QAudio::State)));

    qDebug() << "start humming...";
    _audio_out->start(_wav);
}

void QtPlayer::stop()
{
    QMutexLocker lock(&_mutex);
    _stop();
}

void QtPlayer::_stop()
{
    if (_audio_out) {
        qDebug() << "Stopping...";
        _audio_out->disconnect(this);
        _audio_out->stop();
        _af->close();
        delete _audio_out;
        delete _af;
        _audio_out = 0;
        _af = 0;
        qDebug() << "Stopped";
    }
    else {
        qDebug() << "Nothing to stop";
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

void QtPlayer::_finished_humming(QAudio::State state)
{
    if (state == QAudio::IdleState || state == QAudio::StoppedState) {
        delete _audio_out;
        delete _wav;
        _audio_out = 0;
        _wav = 0;
    }
}
