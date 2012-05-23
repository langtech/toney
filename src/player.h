#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>

class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(QObject *parent = 0);
    virtual void play(const QString& path, double start, double end) = 0;
    virtual void stop() = 0;
    static void setVolumeLevel(int v) { _volume_level = v; }
    static int getVolumeLevel() { return _volume_level; }

signals:
    void finishedPlaying(const QString &path, double start, double end);

public slots:

protected:
    static int _volume_level;
};

#endif // PLAYER_H
