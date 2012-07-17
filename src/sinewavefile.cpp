#include "sinewavefile.h"

static const float PI = 3.14159265;
static const int SAMPLERATE = 16000;
static const float AMPLITUDE = 7000.0;

SineWaveFile::SineWaveFile(QObject *parent) :
    QIODevice(parent),
    _samples(0),
    _n(0),
    _dur(0.0),
    _theta(0.0)
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
    _theta(0.0)
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
    /*
    float samplerate = (float) SAMPLERATE;
    float c = PI * 2.0 / samplerate;
    int i = 0;
    float t = 0.0;
    float theta = 0.0;
    float step = _dur / 30.0;
    int n =
    short buf[(int) (step * samplerate)];
    int counter = 0;
    for (; i < obj->wptr; ++i) {
      float delta = c * obj->data[i];
      if (delta < 0.00001)
        theta = 0.0;
      counter = 0;
      while (t < i * step) {
        float v = sin(theta) * AMPLITUDE;
        buf[counter++] = v;
        //printf("%f %f\n", t,v);
        theta += delta;
        theta -= floor(theta / (2.0 * pi)) * 2.0 * pi;
        t += 1.0 / samplerate;
      }
      sf_writef_short(sndfile, buf, counter);
    }

    sf_close(sndfile);
    */
    for (qint64 i=0; i < maxlen; ++i)
        data[i] = 0;
    return maxlen;
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
