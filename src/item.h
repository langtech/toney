#ifndef ITEM_H
#define ITEM_H

#include <QtGui>
#include <QtWidgets>
#include <QSound>
#include <annotation.h>

class Item : public QGraphicsTextItem
{
public:
    Item();
    ~Item();
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget);
    void setBackgroundColor(const QColor &color);
    const QPixmap *screenshot();

protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    void _make_screen_shot();
    QPixmap *_screenshot;
    QColor _color;

};

#endif // ITEM_H
