#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>

class Player : public QObject
{
    Q_OBJECT
public:
    static Player *getInstance(QObject *parent=0);
    virtual void play(const QString& path, double start, double end) = 0;
    virtual void stop() = 0;
    static void setVolumeLevel(int v) { _volume_level = v; }
    static int getVolumeLevel() { return _volume_level; }

signals:
    void finishedPlaying(const QString &path, double start, double end);

public slots:

protected:
    explicit Player(QObject *parent = 0);
    virtual ~Player();
    static void registerInstance(Player *player);

protected:
    static int _volume_level;
    static Player *_instance;
};

#endif // PLAYER_H
