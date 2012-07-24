#include "sinewavefile.h"

static const float PI = 3.14159265;
static const float AMPLITUDE = 7000.0;

SineWaveFile::SineWaveFile(QObject *parent) :
    QIODevice(parent),
    _samplerate(16000),
    _inc(2.0 * PI / _samplerate),
    _collar(_samplerate / 50),
    _samples(0),
    _samples_size(0),
    _dur(0.0),
    _theta(0.0),
    _i(0),
    _total(0)
{
    QIODevice::open(QIODevice::ReadOnly);
    _setFormat();
}

SineWaveFile::SineWaveFile(
        const float *frequency_samples,
        int n,
        float duration,
        QObject *parent) :
    QIODevice(parent),
    _samplerate(16000),
    _inc(2.0 * PI / _samplerate),
    _collar(_samplerate / 50),
    _samples(frequency_samples),
    _samples_size(n),
    _dur(duration),
    _theta(0.0),
    _i(0),
    _total(_dur * _samplerate)
{
    QIODevice::open(QIODevice::ReadOnly);
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

    if (_samples_size <= 0) {
        for (; _i < N; _i++)
            buf[count++] = 0;
        return count * sizeof(short);
    }
    else {
        for (; _i < N && _i < _collar; _i++) {
            buf[count++] = sin(_theta) * AMPLITUDE / _collar * _i;
            _updateTheta();
        }
        for (; _i < N && _i >= _collar && _i < _total - _collar; _i++) {
            buf[count++] = sin(_theta) * AMPLITUDE;
            _updateTheta();
        }
        for (; _i < N && _i >= _total - _collar; _i++) {
            buf[count++] = sin(_theta) * AMPLITUDE / _collar * (_total-_i);
            _updateTheta();
        }
    }

    return count * sizeof(short);
}

qint64 SineWaveFile::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    return len;
}

qint64 SineWaveFile::bytesAvailable() const
{
    return 0;
}

void SineWaveFile::_setFormat()
{
    _format.setSampleRate(_samplerate);
    _format.setSampleSize(sizeof(short) * 8);
    _format.setSampleType(QAudioFormat::SignedInt);
    _format.setChannels(1);
    _format.setCodec("audio/pcm");

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(_format)) {
        _format = info.nearestFormat(_format);
        _samplerate = _format.sampleRate();
        _inc = 2.0 * PI / _samplerate;
        _collar = _samplerate / 50;
    }

    // sorry this is bad and should be fixed
    assert(_format.channelCount() == 1);
}

void SineWaveFile::_updateTheta()
{
    int fi = _i * _samples_size / _total;  // frequency sample number
    _theta += _inc * _samples[fi];
    _theta -= floor(_theta / 2.0 / PI) * 2.0 * PI;
}
