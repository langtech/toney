#ifndef POOL_H
#define POOL_H

#include <QtGui>
#include <annotationset.h>
#include <item.h>
#include <config.h>

class Pool : public QGraphicsView
{
public:
    explicit Pool(QWidget *parent = 0);
    ~Pool();

    void addAnnotation(const Annotation& ann);
    void setConfig(const Config &config);
    void clear(const AnnotationSet *anns);
    void doColoring();

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void contextMenuEvent(QContextMenuEvent *event);

private:
    QHash<Annotation,Item*> _anns;
    // _anns: dictionary holding all Annotations and corresponding scene
    //        items. Used only for efficient tracking
    QGraphicsScene _scene;
    Item *_drag_item;
    QPoint _drag_start_pos;
    Config _config;

    Annotation _item2ann(QGraphicsItem *item);
    QString _label(const Annotation &ann);
};

#endif // POOL_H
