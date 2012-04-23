#ifndef PHONONPLAYER_H
#define PHONONPLAYER_H

#include <phonon>
#include <player.h>

class PhononPlayer : public Player
{
    Q_OBJECT
public:
    explicit PhononPlayer(QObject *parent = 0);
    virtual ~PhononPlayer();

public slots:
    void play(const QString& path, double start, double end);
    void stop();

private slots:
    void _check_to_stop(qint64);
    void _state_changed(Phonon::State, Phonon::State);

private:
    Phonon::MediaSource *_ms;
    Phonon::MediaObject *_mo;
    QString _path;
    double _start;
    double _end;
};

#endif // PHONONPLAYER_H
