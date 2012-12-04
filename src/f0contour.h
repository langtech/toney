#ifndef F0CONTOUR_H
#define F0CONTOUR_H

#include <QtGui>
#include <annotation.h>

class F0Contour : public QGraphicsPathItem
{
public:
    F0Contour(const Annotation &ann, QGraphicsItem *parent=0);
    ~F0Contour();
    void setHighlighted(bool selected);
    bool isHighlighted() const;
    int type() const { return UserType + 2; }
    bool computeF0();

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    void _redraw();

private:
    Annotation _ann;
    QPen _pen_selected;
    QPen _pen_hover;
    QPen _pen;
    bool _highlighted;
};

#endif // F0CONTOUR_H
