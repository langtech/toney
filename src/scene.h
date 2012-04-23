#ifndef SCENE_H
#define SCENE_H

#include <QtGui>

class Scene : public QGraphicsScene
{
public:
    Scene();

protected:
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dropEvent(QGraphicsSceneDragDropEvent *event);
};

#endif // SCENE_H
