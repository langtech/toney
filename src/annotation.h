#ifndef ANNOTATION_H
#define ANNOTATION_H

#include <QHash>
#include <QMetaType>
#include <tonesexception.h>
#include <QUrl>
#include <QAction>
#if defined(QTPLAYER)
#include "qtplayer.h"
#define PLAYER QtPlayer
#elif defined(PHONONPLAYER)
#include "phononplayer.h"
#define PLAYER PhononPlayer
#else
#include "soxplayer.h"
#define PLAYER SoxPlayer
#endif

#if (QT_POINTER_SIZE == 4)
    typedef quint32 ptr_t;
#else
    typedef quint64 ptr_t;
#endif

typedef struct {
    QString label;      // label on target
    double start;       // start time on target
    double end;         // end time on target
    QString frm_label;  // label on frame (sentence; parent of the target)
    double frm_start;   // start time of the frame (sentence)
    double frm_end;     // end time of the frame (sentence)
    int ann_path;       // path to the annotation file
    int audio_path;     // path to the audio file containing target & frame
    int spkr;
    int tone;
    float f0[30];
    int pitch_tracked;  // whether pitch traking is done (1) or not (0)
    int modified;       // whether annotation has been modified (1) or not (0)
                        //   this is set when tone annotation changes
} _annotation_t;

class InvalidAnnotationId: public TonesException
{
public:
    InvalidAnnotationId(const QString &msg): TonesException(msg) {}
};

class Annotation
{
public:
    Annotation();
    Annotation(const Annotation& ann);
    Annotation(const QByteArray& annId) throw(InvalidAnnotationId);

    ~Annotation();

    Annotation &operator=(const Annotation& ann);
    Annotation clone();

    void setAnnotationFilePath(const QString &ann_path);
    void setAudioPath(const QString& audio_path);
    void setFrame(const QString& label, double start, double end);
    void setTarget(const QString& label, double start, double end);
    void setSpeaker(const QString& spkr);
    void setTone(const QString& tone);
    void clearTone();
    void setF0(const QVector<float>& data);
    void clearPitch();
    void resetModificationFlag();

    const QString& getAnnotationFilePath() const;
    const QString& getAudioPath() const;
    const QString& getFrameLabel() const;
    double getFrameStart() const;
    double getFrameEnd() const;
    const QString& getTargetLabel() const;
    double getTargetStart() const;
    double getTargetEnd() const;
    const QString& getSpeaker() const;
    const QString& getTone() const;
    const float *getF0() const;
    QByteArray getId() const;
    bool modified() const;

    void playTarget(QAction *act=0);
    void playFrame();

    bool operator ==(const Annotation& ann) const;

private:
    static QStringList _ann_paths;
    static QStringList _audio_paths;
    static QStringList _speakers;
    static QStringList _tones;
    static QHash<_annotation_t*,int>  _ref_counter;
    static QString _empty_string;
    static PLAYER *_player;
    _annotation_t *_ann;
};

inline uint qHash(Annotation const& key)
{
    return qHash(QUrl(key.getId()));
}

Q_DECLARE_METATYPE(Annotation)

QDataStream &operator<<(QDataStream &out, const Annotation &ann);
QDataStream &operator>>(QDataStream &in, Annotation &ann);

#endif // ANNOTATION_H
