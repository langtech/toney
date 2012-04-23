#include "scene.h"

Scene::Scene()
{
}

void Scene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    qDebug(event->mimeData()->text().toAscii());
}

void Scene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->scenePos();
}

void Scene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    qDebug("scene");
}
