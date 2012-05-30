#ifndef LISTPLAYER_H
#define LISTPLAYER_H

#include <annotation.h>
#include <targetlist.h>
#include <config.h>

class ListPlayer : public QObject
{
    Q_OBJECT
public:
    typedef enum {STOPPED, PLAYING, PAUSED, SLEEP} STATE;
    typedef enum {TARGET, FRAME} MODE;

    explicit ListPlayer(QObject *parent = 0);
    ~ListPlayer();
    void setList(TargetList *list);
    void setMode(MODE mode);
    void play();
    void pause();
    void resume();
    void stop();
    STATE state();
    MODE mode();
    void setConfig(const Config &config);

signals:
    void finishedPlaying();

private slots:
    void _finished_playing();
    void _next();

private:
    Config _config;
    TargetList *_list;
    STATE _state;
    int _idx;
    MODE _mode;
    Player *_player;
};

#endif // LISTPLAYER_H
