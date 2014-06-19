#include <iostream>
#include "audiofile.h"
#include "player.h"

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
        std::cout << "error 1" << std::endl;
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
        std::cout << "error 2" << std::endl;
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
#ifdef Q_WS_WIN
        // This seems to be a Qt bug. The manual says this method returns -1
        // if there's no more data to be read. However, if we do that,
        // QAudioOutput doesn't change state to either Idle or Stopped.
        return 0;
#else
        return -1;
#endif

    if (maxlen <= 0)
        return 0;

    // compute the number of frames to read
    sf_count_t frames = maxlen / sizeof(short) / _sfinfo.channels;
    if (frames > _region_frames)
        frames = _region_frames;

    short *tmp = new short[frames * _sfinfo.channels];

    sf_count_t n = sf_readf_short(_sndfile, tmp, frames);

    // compute multiplier
    double dB = Player::getVolumeLevel();
    double multiplier = pow(10.0, dB / 20.0);

    // adjust multiplier to avoid clipping
    short maxval = SHRT_MIN;
    for (int i=0; i < n * _sfinfo.channels; ++i)
        if (tmp[i] > maxval)
            maxval = tmp[i];
    if (multiplier * maxval > SHRT_MAX)
        multiplier = SHRT_MAX / maxval;

    // apply multiplier
    for (int i=0; i < n * _sfinfo.channels; ++i)
        ((short*)data)[i] = (int) (multiplier * tmp[i] + 0.5);

    _counter += n;

    delete[] tmp;
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
