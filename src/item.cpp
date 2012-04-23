#include "item.h"
#include <QMimeData>
#include <QDrag>

Item::Item():
    _screenshot(0)
{
    this->setDefaultTextColor(Qt::black);
}

Item::~Item()
{
    if (_screenshot != 0)
        delete _screenshot;
}

const QPixmap *Item::screenshot()
{
    if (_screenshot == 0) {
        _make_screen_shot();
    }
    return _screenshot;
}

void Item::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    this->setDefaultTextColor(Qt::blue);
}

void Item::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    this->setDefaultTextColor(Qt::black);
}

void Item::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    QVariant v(this->data(0));
    Annotation ann(v.value<Annotation>());
    ann.playTarget();
}

void Item::_make_screen_shot()
{
    if (_screenshot != 0) {
        delete _screenshot;
    }
    _screenshot = new QPixmap(this->boundingRect().size().toSize());
    _screenshot->fill(QColor(255,255,255,127));
    QPainter painter(_screenshot);
    QStyleOptionGraphicsItem style;
    paint(&painter, &style, 0);
    painter.end();
}
