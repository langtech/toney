#include "f0contour.h"
#include "playmenu.h"
#include <sndfile.h>
#include <QDebug>

static void callback(
        float *f0p,
        float *vuvp,
        float *rms_speech,
        float *acpkp,
        int vecsize,
        void *userdata)
{
    Q_UNUSED(rms_speech);
    Q_UNUSED(acpkp);
    QVector<float> *v = (QVector<float>*) userdata;
    for (int i=0; i < vecsize; ++i) {
        v->push_front(vuvp[i]);
        v->push_front(f0p[i]);
    }
}

static bool pitch_track(Annotation &ann, get_f0_session *session)
{
    QString p = ann.getAudioPath();
    if (p.isEmpty())
        return false;

    SF_INFO sfinfo;
    SNDFILE *sndfile = sf_open(p.toUtf8().constData(), SFM_READ, &sfinfo);
    if (sndfile == 0)
        return false;
    sf_close(sndfile);

    double fend = ((double) sfinfo.frames) / sfinfo.samplerate;
    double beg = ann.getTargetStart() - 0.5;
    double end = ann.getTargetEnd() + 0.5;
    if (beg < 0.0)
        beg = 0.0;
    if (end > fend)
        end = fend;

    QVector<float> v;
    get_f0(p.toUtf8().constData(), session, beg, end, callback, &v);

    double t0 = ann.getTargetStart() - beg;
    double step = (ann.getTargetEnd() - ann.getTargetStart()) / 29.0;
    QVector<float> pitch_sample;
    for (int i=0; i < 30; ++i) {
        double t = t0 + step * i;
        double x = t / session->par->frame_step;
        double x1 = floor(x);
        double x2 = ceil(x);
        double x0 = x1 - 1.0;
        double x3 = x2 + 1.0;
        int i0 = int(x0) * 2;
        int i1 = int(x1) * 2;
        int i2 = int(x2) * 2;
        int i3 = int(x3) * 2;
        float vp0 = i0 >= 0 ? v.at(i0+1) : 0.0;
        float vp1 = v.at(i1+1);
        float vp2 = v.at(i2+1);
        float vp3 = i3 < 60 ? v.at(i3+1) : 0.0;
        double y;

        if (vp1 < 0.5) {
            // x1 unvoiced
            if (vp2 < 0.5) {
                // x1, x2 unvoiced
                y = -1.0;
            }
            else {
                // x1 unvoiced, x2 voiced
                if (x - x1 < 0.5) {
                    // x is closer to unvoiced x1
                    y = -1.0;
                }
                else {
                    // x is closer to voiced x2
                    if (vp3 < 0.5) {
                        // x3 is unvoiced or ob
                        y = v.at(i2);
                    }
                    else {
                        // x3 is voiced
                        // extrapolate with x2 and x3
                        int y2 = v.at(i2);
                        int y3 = v.at(i3);
                        y = (x3*y2 - x2*y3 + (y3-y2)*x) / (x3 - x2);
                    }
                }
            }
        }
        else {
            // x1 is voiced
            if (vp2 < 0.5) {
                // x1 voiced, x2 unvoiced
                if (x2 - x < 0.5) {
                    // x is closer to unvoiced x2
                    y = -1.0;
                }
                else {
                    // x is closer to voiced x1
                    if (vp0 < 0.5) {
                        // x0 is unvoiced or ob
                        y = v.at(i1);
                    }
                    else {
                        // x0 is voiced
                        // extraploate with x0 and x1
                        int y0 = v.at(i0);
                        int y1 = v.at(i1);
                        y = (x1*y0 - x0*y1 + (y1-y0)*x) / (x1 - x0);
                    }
                }
            }
            else {
                // interpolate with x1 and x2
                double y1 = v.at(i1);
                double y2 = v.at(i2);
                y = (y2 - y1) / (x2 - x1) * (x - x1) + y1;
            }
        }
        pitch_sample.push_back((float) y);
    }
    ann.setF0(pitch_sample);

    return true;
}

F0Contour::F0Contour(const Annotation &ann, QGraphicsItem *parent) :
    QGraphicsPathItem(parent),
    _ann(ann),
    _pen_selected(Qt::darkBlue),
    _pen_hover(Qt::green),
    _pen(Qt::black),
    _highlighted(false),
    _f0_session(0)
{
    _pen_selected.setCosmetic(true);
    _pen_selected.setWidth(5);
    _pen_selected.setCapStyle(Qt::RoundCap);
    _pen_hover.setCosmetic(true);
    _pen_hover.setWidth(5);
    _pen_hover.setCapStyle(Qt::RoundCap);
    setAcceptsHoverEvents(true);
    _redraw();
}

F0Contour::~F0Contour()
{
    if (_f0_session)
        close_get_f0(_f0_session);
}

void F0Contour::setHighlighted(bool selected)
{
    QGraphicsPathItem::setSelected(selected);
    if (selected) {
        setPen(_pen_selected);
        setZValue(1.0);
    }
    else {
        setPen(_pen);
        setZValue(0.0);
    }
    _highlighted = selected;
}

bool F0Contour::isHighlighted() const
{
    return _highlighted;
}

bool F0Contour::computeF0()
{
    if (_f0_session == 0) {
        _f0_session = init_get_f0();
        _f0_session->par->min_f0 = 60.0;
        _f0_session->par->max_f0 = 650.0;
        _f0_session->par->wind_dur = 0.01;
        _f0_session->par->frame_step = 0.005;
    }

    if (pitch_track(_ann, _f0_session)) {
        _redraw();
        return true;
    }
    else
        return false;
}

void F0Contour::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    setPen(_pen_hover);
}

void F0Contour::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    if (_highlighted)
        setPen(_pen_selected);
    else
        setPen(_pen);
}

void F0Contour::_redraw()
{
    QPainterPath path;
    const float *data = _ann.getF0();
    if (data) {
        for (int i=1; i < 30; ++i) {
            if (data[i] > 0.0 && data[i-1] > 0.0) {
                path.moveTo((i-1)*10, -data[i-1]);
                path.lineTo(i*10, -data[i]);
            }
        }
    }
    setPath(path);
}
