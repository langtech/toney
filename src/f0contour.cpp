#include "f0contour.h"
#include "playmenu.h"
#include <QDebug>


F0Contour::F0Contour(const Annotation &ann, QGraphicsItem *parent) :
    QGraphicsPathItem(parent),
    _ann(ann),
    _pen_selected(Qt::darkBlue),
    _pen_hover(Qt::green),
    _pen(Qt::black),
    _highlighted(false)
{
    _pen_selected.setCosmetic(true);
    _pen_selected.setWidth(5);
    _pen_selected.setCapStyle(Qt::RoundCap);
    _pen_hover.setCosmetic(true);
    _pen_hover.setWidth(5);
    _pen_hover.setCapStyle(Qt::RoundCap);
    setAcceptHoverEvents(true);
    _redraw();
}

F0Contour::~F0Contour()
{
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
    if (_ann.getF0() != 0) {
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
