#include "annotation.h"
#include "toney_utils.h"
#include <QDir>
#include <sndfile.h>
#include <QDebug>

QStringList Annotation::_ann_paths;
QStringList Annotation::_audio_paths;
QStringList Annotation::_speakers;
QStringList Annotation::_values;
QHash<_annotation_t*,int> Annotation::_ref_counter;
QString Annotation::_empty_string;
int Annotation::NUM_F0_SAMPLES = _ANN_NUM_F0_SAMPLES;

Annotation::Annotation():
    _ann(new _annotation_t)
{
    _ann->ann_path = -1;
    _ann->audio_path = -1;
    _ann->spkr = -1;
    _ann->frm_start = -1.0;
    _ann->frm_end = -1.0;
    _ann->start = -1.0;
    _ann->end = -1.0;
    _ann->pitch_tracked = 0;
    _ann->modified = 0;
    _ref_counter.insert(_ann, 1);
}

Annotation::Annotation(const Annotation& ann)
{
    _ann = ann._ann;
    // increase the counter
    _ref_counter.insert(_ann, _ref_counter.value(_ann) + 1);
}

Annotation::Annotation(const QByteArray &annId) throw(InvalidAnnotationId)
{
    ptr_t x = 0;
    for (int i=0; i < QT_POINTER_SIZE; ++i) {
        x = x << 8;
        x = x | (unsigned char) annId.at(i);
    }
    _annotation_t *p = (_annotation_t*) x;
    if (_ref_counter.find(p) == _ref_counter.end())
        throw InvalidAnnotationId("");

    _ann = p;
    _ref_counter.insert(_ann, _ref_counter.value(_ann) + 1);
}

Annotation::~Annotation()
{
    if (_ref_counter.value(_ann) <= 1) {
        // this is the last instance; deallocate
        _ref_counter.remove(_ann);
        delete _ann;
    } else {
        // this is not the last one; decrease the counter
        _ref_counter.insert(_ann, _ref_counter.value(_ann) - 1);
    }
}

Annotation& Annotation::operator =(const Annotation& ann)
{
    if (_ref_counter.value(_ann) <= 1) {
        // this is the last instance; deallocate
        _ref_counter.remove(_ann);
        delete _ann;
    } else {
        // this is not the last one; decrease the counter
        _ref_counter.insert(_ann, _ref_counter.value(_ann) - 1);
    }
    _ann = ann._ann;
    _ref_counter.insert(_ann, _ref_counter.value(_ann) + 1);
    return *this;
}

Annotation Annotation::clone()
{
    Annotation ann;
    ann._ann->label = _ann->label;
    ann._ann->start = _ann->start;
    ann._ann->end = _ann->end;
    ann._ann->frm_label = _ann->frm_label;
    ann._ann->frm_start = _ann->frm_start;
    ann._ann->frm_end = _ann->frm_end;
    ann._ann->ann_path = _ann->ann_path;
    ann._ann->audio_path = _ann->audio_path;
    ann._ann->spkr = _ann->spkr;
    QHash<int,int>::iterator i;
    for (i = _ann->values.begin(); i != _ann->values.end(); ++i)
        ann._ann->values[i.key()] = i.value();
    for (i = _ann->values2.begin(); i != _ann->values2.end(); ++i)
        ann._ann->values2[i.key()] = i.value();
    ann._ann->pitch_tracked = _ann->pitch_tracked;
    ann._ann->modified = _ann->modified;
    for (int i=0; i < NUM_F0_SAMPLES; ++i)
        ann._ann->f0[i] = _ann->f0[i];
    return ann;
}

void Annotation::setAnnotationFilePath(const QString &path)
{
    int idx = _ann_paths.indexOf(path);
    if (idx >= 0) {
        _ann->ann_path = idx;
    } else {
        _ann->ann_path = _ann_paths.size();
        _ann_paths.append(path);
    }
    clearPitch();
}

void Annotation::setAudioPath(const QString& path)
{
    int idx = _audio_paths.indexOf(path);
    if (idx >= 0) {
        _ann->audio_path = idx;
    } else {
        _ann->audio_path = _audio_paths.size();
        _audio_paths.append(path);
    }
    clearPitch();
}

void Annotation::setFrame(const QString& label, double start, double end)
{
    _ann->frm_label = label;
    _ann->frm_start = start;
    _ann->frm_end = end;
}

void Annotation::setTarget(const QString& label, double start, double end)
{
    _ann->label = label;
    _ann->start = start;
    _ann->end = end;
}

void Annotation::setSpeaker(const QString& spkr)
{
    int idx = _speakers.indexOf(spkr);
    if (idx >= 0) {
        _ann->spkr = idx;
    } else {
        _ann->spkr = _speakers.size();
        _speakers.append(spkr);
    }
}

void Annotation::setValue(int pos, const QString &value)
{
    int idx = _values.indexOf(value);
    if (idx >= 0) {
        _ann->values[pos] = idx;
    }
    else {
        _ann->values[pos] = _values.size();
        _values.append(value);
    }
}

void Annotation::setValue2(int pos, const QString &value)
{
    int idx = _values.indexOf(value);
    if (idx >= 0)
        _ann->values2[pos] = idx;
}

void Annotation::clearValue(int pos)
{
    _ann->values.remove(pos);
}

void Annotation::clearValue2(int pos)
{
    _ann->values2.remove(pos);
}

void Annotation::setF0(const QVector<float> &data)
{
    if (data.size() < NUM_F0_SAMPLES)
        return;
    for (int i=0; i < NUM_F0_SAMPLES; ++i)
        _ann->f0[i] = data.at(i);
    _ann->pitch_tracked = 1;
}

void Annotation::clearPitch()
{
    _ann->pitch_tracked = 0;
}

void Annotation::resetModificationFlag()
{
    _ann->modified = 0;
}

const QString& Annotation::getAnnotationFilePath() const
{
    if (_ann->ann_path >= 0) {
        return _ann_paths.at(_ann->ann_path);
    } else {
        return _empty_string;
    }
}

const QString& Annotation::getAudioPath() const
{
    if (_ann->audio_path >= 0) {
        return _audio_paths.at(_ann->audio_path);
    } else {
        return _empty_string;
    }
}

const QString& Annotation::getFrameLabel() const
{
    return _ann->frm_label;
}

// Returns -1.0 if the time is not set
double Annotation::getFrameStart() const
{
    return _ann->frm_start;
}

// Returns -1.0 if the time is not set
double Annotation::getFrameEnd() const
{
    return _ann->frm_end;
}

const QString& Annotation::getTargetLabel() const
{
    return _ann->label;
}

// Returns -1.0 if the time is not set
double Annotation::getTargetStart() const
{
    return _ann->start;
}

// Returns -1.0 if the time is not set
double Annotation::getTargetEnd() const
{
    return _ann->end;
}

const QString& Annotation::getSpeaker() const
{
    if (_ann->spkr >= 0) {
        return _speakers.at(_ann->spkr);
    } else {
        return _empty_string;
    }
}

const QString &Annotation::getValue(int pos) const
{
    int index = _ann->values.value(pos, -1);
    if (index >= 0)
        return _values.at(index);
    else
        return _empty_string;
}

const QString &Annotation::getValue2(int pos) const
{
    int index = _ann->values2.value(pos, -1);
    if (index >= 0)
        return _values.at(index);
    else
        return _empty_string;
}

const float* Annotation::getF0() const
{
    if (_ann->pitch_tracked)
        return _ann->f0;
    else {
        float *samples = 0;
        get_f0_session *session = COM.newGetF0Session();
        if (session &&
                get_f0_samples(
                    getAudioPath().toUtf8().data(),
                    _ann->start,
                    _ann->end,
                    _ann->f0,
                    NUM_F0_SAMPLES,
                    session))
        {
            _ann->pitch_tracked = 1;
            samples = _ann->f0;
            close_get_f0(session);
        }
        return samples;
    }
}

bool Annotation::f0Computed() const
{
    return _ann->pitch_tracked == 1;
}

QByteArray Annotation::getId() const
{
    ptr_t p = (ptr_t) _ann;
    QByteArray ba;
    for (int i=0; i < QT_POINTER_SIZE; ++i) {
        ba.prepend(static_cast<char>(p & 0xff));
        p = p >> 8;
    }
    return ba;
}

bool Annotation::modified() const
{
    return _ann->modified == 1;
}

void Annotation::playTarget(QAction *act)
{
    if (_ann->audio_path < 0 || _ann->start < 0.0 || _ann->end < 0.0)
        // audio region is not defined
        return;

    Player *player = Player::getInstance();

    if (act != 0) {
        player->disconnect(act);
        act->connect(player, SIGNAL(finishedPlaying(QString,double,double)),
                     SLOT(trigger()));
    }

    player->play(getAudioPath(), _ann->start, _ann->end);
}

void Annotation::playFrame()
{
    if (_ann->audio_path < 0 || _ann->frm_start < 0.0 || _ann->frm_end < 0.0)
        // audio region is not defined
        return;

    PLAYER::getInstance()->play(
                getAudioPath(), _ann->frm_start, _ann->frm_end);
}

void Annotation::hum()
{
    if (_ann->pitch_tracked == 0)
        return;

    PLAYER::getInstance()->hum(
                _ann->f0, NUM_F0_SAMPLES, _ann->start, _ann->end);
}

int Annotation::min_sample_rate()
{
    int min_val = 9999999;
    bool changed = false;
    foreach (QString path, _audio_paths) {
        const char *p = path.toUtf8().constData();
        int r = sample_rate(p);
        if (min_val > r) {
            min_val = r;
            changed = true;
        }
    }

    if (changed)
        return min_val;
    else
        return -1;
}

bool Annotation::operator ==(const Annotation& ann) const
{
    return ann._ann == _ann;
    /*
    return _ann->frm_label == ann._ann->frm_label &&
           _ann->frm_start == ann._ann->frm_start &&
           _ann->frm_end == ann._ann->frm_end &&
           _ann->label == ann._ann->label &&
           _ann->start == ann._ann->start &&
           _ann->end == ann._ann->end &&
           _ann->audio_path == ann._ann->audio_path &&
           _ann->spkr == ann._ann->spkr &&
           _ann->tone == ann._ann->tone;
           */
}

QDataStream &operator <<(QDataStream &out, const Annotation &ann)
{
    out << ann.getId();
    return out;
}

QDataStream &operator>>(QDataStream &in, Annotation &ann)
{
    QByteArray ba;
    in >> ba;
    ann = ba;
    return in;
}

