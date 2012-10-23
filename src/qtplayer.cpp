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
    if (!_mutex.tryLock())
        return;

    if (_audio_out)
        _stop();

    AudioFile *af = new AudioFile(this);

    if (af->open(path) == false) {
        qDebug("failed to open audio file: %s: %s",
               path.toUtf8().constData(),
               af->lastError().toUtf8().constData());
        delete af;
        _mutex.unlock();
        return;
    }

    _af = dynamic_cast<QIODevice*>(af);

    af->setRegion(start, end);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(af->format())) {
        _mutex.unlock();
        return;
    }

    _path = path;
    _start = start;
    _end = end;

    _audio_out = new QAudioOutput(info, af->format(), this);

    connect(_audio_out, SIGNAL(stateChanged(QAudio::State)),
            this, SLOT(_finished_playing(QAudio::State)));

    _audio_out->start(_af);

    _mutex.unlock();
}

void QtPlayer::hum(float *f0_samples, int nsamp, double start, double end)
{
    if (!_mutex.tryLock())
        return;

    if (_audio_out)
        _stop();


    SineWaveFile *wav = new SineWaveFile(f0_samples, nsamp, end - start, this);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(wav->format())) {
        delete wav;
        _mutex.unlock();
        return;
    }

    _af = dynamic_cast<QIODevice*>(wav);

    _audio_out = new QAudioOutput(info, wav->format(), this);

    connect(_audio_out, SIGNAL(stateChanged(QAudio::State)),
            this, SLOT(_finished_playing(QAudio::State)));

    _audio_out->start(_af);
    _mutex.unlock();
}

void QtPlayer::stop()
{
    if (!_mutex.tryLock())
        return;
    _stop();
    _mutex.unlock();
}

void QtPlayer::_stop()
{
    if (_audio_out) {
        _audio_out->disconnect(this);
        _audio_out->stop();
        _af->close();
        delete _audio_out;
        delete _af;
        _audio_out = 0;
        _af = 0;
    }
}

void QtPlayer::_finished_playing(QAudio::State state)
{
    if (state == QAudio::IdleState || state == QAudio::StoppedState) {
        stop();
        emit finishedPlaying(_path, _start, _end);
    }
}
