#ifndef AUDIOFILE_H
#define AUDIOFILE_H

#include <QIODevice>
#include <QtMultimedia>
#include <sndfile.h>

class AudioFile : public QIODevice
{
    Q_OBJECT

public:
    AudioFile(QObject *parent=0);
    ~AudioFile();

    bool open(const QString &path);
    const QString &lastError() const;
    const QAudioFormat &format() const;


    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);
    qint64 bytesAvailable() const;
    void setRegion(double start, double end);
    void setMultiplier(double m);

private:
    SF_INFO _sfinfo;
    SNDFILE *_sndfile;
    sf_count_t _counter;
    QString _last_error_msg;
    QAudioFormat _format;
    sf_count_t _region_frames;
    double _multiplier;
    int _empty_read;  // count attempts to read beyond eof
};

#endif // AUDIOFILE_H
