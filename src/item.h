#ifndef ITEM_H
#define ITEM_H

#include <QtGui>
#include <QSound>
#include <annotation.h>

class Item : public QGraphicsTextItem
{
public:
    Item();
    ~Item();
    const QPixmap *screenshot();

protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    void _make_screen_shot();
    QPixmap *_screenshot;

};

#endif // ITEM_H
