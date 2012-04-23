#include "phononplayer.h"
#include <sndfile.h>

PhononPlayer::PhononPlayer(QObject *parent) :
    Player(parent),
    _ms(0),
    _mo(0)
{
}

PhononPlayer::~PhononPlayer()
{
    if (_ms)
        delete _ms;
    if (_mo)
        delete _mo;
}

void PhononPlayer::play(const QString &path, double start, double end)
{
    if (_ms)
        delete _ms;
    if (_mo)
        delete _mo;

    _path = path;
    _start = start;
    _end = end;

    _ms = new Phonon::MediaSource(path);
    _mo = Phonon::createPlayer(Phonon::MusicCategory, *_ms);
    _mo->setTickInterval(15);

    connect(_mo, SIGNAL(tick(qint64)),
            SLOT(_check_to_stop(qint64)));

    connect(_mo, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            SLOT(_state_changed(Phonon::State,Phonon::State)));

    _mo->play();
}

void PhononPlayer::stop()
{
    if (_mo)
        _mo->stop();
}

void PhononPlayer::_check_to_stop(qint64 t)
{
    qDebug() << t;
    if (t >= _end * 1000) {
        _mo->stop();
        emit finishedPlaying(_path, _start, _end);
    }
}

void PhononPlayer::_state_changed(Phonon::State new_state, Phonon::State old_state)
{
    qDebug() << new_state;
    if (new_state == Phonon::LoadingState ||
            new_state == Phonon::PlayingState)
        _mo->seek(_start * 1000);
}
