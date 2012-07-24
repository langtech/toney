#ifndef SINEWAVEFILE_H
#define SINEWAVEFILE_H

#include <QIODevice>
#include <QtMultimedia>

class SineWaveFile : public QIODevice
{
    Q_OBJECT

public:
    SineWaveFile(QObject *parent=0);
    SineWaveFile(const float *frequency_samples,
                 int n,
                 float duration,
                 QObject *parent=0);
    ~SineWaveFile();

    const QAudioFormat &format() const;
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);
    qint64 bytesAvailable() const;

private:
    void _setFormat();
    void _updateTheta();

private:
    int _samplerate;
    float _inc;
    int _collar;      // width of bases of slopes of trapezoidal window
                      //   that is used for removing clicking sound
    const float *_samples; // frequency samples
    int _samples_size;     // size of _samples
    float _dur;   // duration
    float _theta; // current angle
    int _i;       // sample number
    int _total;   // total number of audio samples to generate
    QAudioFormat _format;
};

#endif // SINEWAVEFILE_H
