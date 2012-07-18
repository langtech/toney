#ifndef QTPLAYER_H
#define QTPLAYER_H

#include <player.h>
#include <QtMultimedia>
#include <audiofile.h>
#include <sinewavefile.h>

class QtPlayer : public Player
{
    Q_OBJECT

    friend class MainWindow;

public slots:
    virtual void play(const QString& path, double start, double end);
    virtual void hum(float *f0_samples, int nsamp, double start, double end);
    virtual void stop();

signals:

protected:
    explicit QtPlayer(QObject *parent = 0);
    ~QtPlayer();

private slots:
    void _finished_playing(QAudio::State state);
    void _finished_humming(QAudio::State state);

private:
    QAudioOutput *_audio_out;
    AudioFile *_af;
    SineWaveFile * _wav;
    QString _path;
    double _start;
    double _end;
    QMutex _mutex;
};

#endif // QTPLAYER_H
