#include "listplayer.h"

ListPlayer::ListPlayer(QObject *parent) :
    QObject(parent),
    _list(0),
    _state(STOPPED),
    _idx(0),
    _mode(TARGET)
{
    connect(&_player,
            SIGNAL(finishedPlaying(QString,double,double)),
            SLOT(_finished_playing()));
}

ListPlayer::~ListPlayer()
{
    _player.stop();
}

void ListPlayer::setList(TargetList *list)
{
    _list = list;
}

void ListPlayer::setMode(ListPlayer::MODE mode)
{
    _mode = mode;
}

void ListPlayer::play()
{
    if (_state == STOPPED) {
        _idx = 0;
        _state = PLAYING;
        _next();
    }
}

void ListPlayer::pause()
{
    if (_state == PLAYING) {
        _player.stop();
        _state = PAUSED;
    }
}

void ListPlayer::resume()
{
    if (_state == PAUSED) {
        _state = PLAYING;
        _next();
    }
}

void ListPlayer::stop()
{
    _player.stop();
    _state = STOPPED;
    _idx = 0;
    emit finishedPlaying();
}

ListPlayer::STATE ListPlayer::state()
{
    return _state;
}

ListPlayer::MODE ListPlayer::mode()
{
    return _mode;
}

void ListPlayer::setConfig(const Config &config)
{
    _config = config;
}

void ListPlayer::_finished_playing()
{
    int delay = _config.playback_delay ? 500 : 0;
    QTimer::singleShot(delay, this, SLOT(_next()));
}

void ListPlayer::_next()
{
    if (_idx >= _list->count()) {
        stop();
    }
    else if (_state == PLAYING) {
        _list->setCurrentRow(_idx);
        Annotation ann = _list->annotation(_idx);
        if (_mode == TARGET) {
            _player.play(ann.getAudioPath(),
                         ann.getTargetStart(),
                         ann.getTargetEnd());
        }
        else if (_mode == FRAME) {
            _player.play(ann.getAudioPath(),
                         ann.getFrameStart(),
                         ann.getFrameEnd());
        }
        _idx++;
    }
}
