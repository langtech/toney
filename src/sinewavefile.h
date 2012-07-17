#ifndef SINEWAVEFILE_H
#define SINEWAVEFILE_H

#include <QIODevice>
#include <QtMultimedia>

class SineWaveFile : public QIODevice
{
    Q_OBJECT

public:
    SineWaveFile(QObject *parent=0);
    SineWaveFile(float *frequency_samples,
                 int n,
                 float duration,
                 QObject *parent=0);
    ~SineWaveFile();

    const QAudioFormat &format() const;
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

private:
    void _setFormat();

private:
    const float *_samples; // frequency samples
    int _n; // number of samples
    float _dur; // duration
    float _theta;
    QAudioFormat _format;
};

#endif // SINEWAVEFILE_H
