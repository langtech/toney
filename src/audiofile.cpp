#include "audiofile.h"

AudioFile::AudioFile(QObject *parent):
    QIODevice(parent),
    _sndfile(0),
    _counter(0)
{
    QIODevice::open(QIODevice::ReadOnly);
}

AudioFile::~AudioFile()
{
    if (_sndfile)
        sf_close(_sndfile);
}

bool AudioFile::open(const QString &path)
{
    if (_sndfile) {
        sf_close(_sndfile);
        _counter = 0;
        _last_error_msg.clear();
    }

    const char *p = path.toUtf8().constData();
    _sndfile = sf_open(p, SFM_READ, &_sfinfo);
    if (_sndfile == 0) {
        _last_error_msg.fromUtf8(sf_strerror(_sndfile));
        return false;
    }

    _format.setSampleRate(_sfinfo.samplerate);
    _format.setSampleSize(sizeof(short) * 8);
    _format.setSampleType(QAudioFormat::SignedInt);
    _format.setChannels(_sfinfo.channels);
    _format.setCodec("audio/pcm");

    if (_format.isValid() == false) {
        sf_close(_sndfile);
        _sndfile = 0;
        return false;
    }

    _region_frames = _sfinfo.frames;

    return true;
}

const QString &AudioFile::lastError() const
{
    return _last_error_msg;
}

const QAudioFormat &AudioFile::format() const
{
    return _format;
}

qint64 AudioFile::readData(char *data, qint64 maxlen)
{
    if (_sndfile == 0)
        return -1;

    if (_counter >= _region_frames)
        return 0;

    if (maxlen <= 0)
        return 0;

    // compute the number of frames to read
    sf_count_t frames = maxlen / sizeof(short) / _sfinfo.channels;
    if (frames > _region_frames)
        frames = _region_frames;

    sf_count_t n = sf_readf_short(_sndfile, (short*) data, frames);
    _counter += n;

    return n * sizeof(short) * _sfinfo.channels;
}

qint64 AudioFile::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);
    return 0;
}

qint64 AudioFile::bytesAvailable() const
{
    if (_sndfile == 0)
        return 0;

    return (_region_frames - _counter) * sizeof(short) * _sfinfo.channels;
}

void AudioFile::close()
{
    qDebug() << "close";
}

void AudioFile::setRegion(double start, double end)
{
    if (_sndfile == 0)
        return;

    if (start < 0.0)
        start = 0.0;

    if (end < 0.0)
        end = 0.0;

    sf_count_t b = start * _sfinfo.samplerate;
    sf_count_t e = end * _sfinfo.samplerate;

    if (e > _sfinfo.frames)
        e = _sfinfo.frames;

    if (b > e)
        b = e;

    _region_frames = e - b;

    sf_seek(_sndfile, b, SEEK_SET);
}
