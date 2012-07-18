#include "sinewavefile.h"

static const float PI = 3.14159265;
static const int SAMPLERATE = 16000;
static const float AMPLITUDE = 7000.0;
static const float INC = PI * 2.0 / SAMPLERATE;

SineWaveFile::SineWaveFile(QObject *parent) :
    QIODevice(parent),
    _samples(0),
    _n(0),
    _dur(0.0),
    _theta(0.0),
    _i(0),
    _total(0)
{
    _setFormat();
}

SineWaveFile::SineWaveFile(
        const float *frequency_samples,
        int n,
        float duration,
        QObject *parent) :
    QIODevice(parent),
    _samples(frequency_samples),
    _n(n),
    _dur(duration),
    _theta(0.0),
    _i(0),
    _total(_dur * SAMPLERATE)
{
    _setFormat();
}

SineWaveFile::~SineWaveFile()
{
}

const QAudioFormat &SineWaveFile::format() const
{
    return _format;
}

qint64 SineWaveFile::readData(char *data, qint64 maxlen)
{
    if (_i >= _total)
        return -1;

    int N = _i + maxlen / sizeof(short);
    if (N > _total)
        N = _total;
    short *buf = (short*) data;
    int count = 0;

    if (_n <= 0) {
        for (; _i < N; _i++)
            buf[count++] = 0;
        return count * sizeof(short);
    }
    else {
        for (; _i < N; _i++) {
            buf[count++] = sin(_theta) * AMPLITUDE;
            int j = (int) floor((_i + k) / _total);
            _theta += INC * _samples[j];
            _theta -= floor(_theta / 2.0 / PI) * 2.0 * PI;
        }
    }

    return count * sizeof(short);
}

qint64 SineWaveFile::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    return len;
}

void SineWaveFile::_setFormat()
{
    _format.setSampleRate(SAMPLERATE);
    _format.setSampleSize(sizeof(short) * 8);
    _format.setSampleType(QAudioFormat::SignedInt);
    _format.setChannels(1);
    _format.setCodec("audio/pcm");
}
