#include "annotation.h"
#include <QDir>
#include <sndfile.h>
#include <QDebug>

QStringList Annotation::_ann_paths;
QStringList Annotation::_audio_paths;
QStringList Annotation::_speakers;
QStringList Annotation::_tones;
QHash<_annotation_t*,int> Annotation::_ref_counter;
QString Annotation::_empty_string;

Annotation::Annotation():
    _ann(new _annotation_t)
{
    _ann->ann_path = -1;
    _ann->audio_path = -1;
    _ann->spkr = -1;
    _ann->tone = -1;
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
    ann._ann->tone = _ann->tone;
    ann._ann->pitch_tracked = _ann->pitch_tracked;
    ann._ann->modified = _ann->modified;
    for (int i=0; i < 30; ++i)
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

void Annotation::setTone(const QString& tone)
{
    int idx = _tones.indexOf(tone);
    if (idx >= 0) {
        _ann->tone = idx;
    } else {
        _ann->tone = _tones.size();
        _tones.append(tone);
    }
    _ann->modified = 1;
}

void Annotation::clearTone()
{
    _ann->tone = -1;
    _ann->modified = 1;
}

void Annotation::setF0(const QVector<float> &data)
{
    if (data.size() < 30)
        return;
    for (int i=0; i < 30; ++i)
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

const QString& Annotation::getTone() const
{
    if (_ann->tone >= 0) {
        return _tones.at(_ann->tone);
    } else {
        return _empty_string;
    }
}

const float* Annotation::getF0() const
{
    if (_ann->pitch_tracked)
        return _ann->f0;
    else
        return 0;
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

    Player *player = PLAYER::getInstance();

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

