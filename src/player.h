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

signals:
    void finishedPlaying(const QString &path, double start, double end);

public slots:

};

#endif // PLAYER_H
