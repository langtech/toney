#include "qtplayer.h"
#include <sndfile.h>

QtPlayer::QtPlayer(QObject *parent) :
    Player(parent),
    _audio_out(0),
    _af(0)
{
}

QtPlayer::~QtPlayer()
{

}

void QtPlayer::play(const QString &path, double start, double end)
{
    if (_audio_out && _audio_out->state() != QAudio::IdleState) {
        _audio_out->stop();
        delete _audio_out;
        delete _af;
        _audio_out = 0;
        _af = 0;
    }

    _af = new AudioFile(this);

    if (_af->open(path) == false) {
        qDebug("failed to open audio file: %s: %s",
               path.toUtf8().constData(),
               _af->lastError().toUtf8().constData());
        return;
    }

    _af->setRegion(start, end);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultInputDevice());
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

void QtPlayer::stop()
{
    if (_audio_out) {
        _audio_out->stop();
        delete _af;
        delete _audio_out;
        _audio_out = 0;
        _af = 0;
    }
}

void QtPlayer::_finished_playing(QAudio::State state)
{
    if (state == QAudio::IdleState) {
        _audio_out->stop();
        delete _af;
        delete _audio_out;
        _audio_out = 0;
        _af = 0;
        emit finishedPlaying(_path, _start, _end);
    }
}
