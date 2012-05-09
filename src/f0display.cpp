#include "f0display.h"
#include "playmenu.h"
#include <QDebug>

F0Display::F0Display(QWidget *parent) :
    QGraphicsView(parent)
{
    setScene(&_scene);
    setAlignment(Qt::AlignBottom | Qt::AlignLeft);
    QPen pen(Qt::transparent);
    _scene.addLine(0,0,290,0, pen);
}

F0Display::~F0Display()
{
}

void F0Display::addAnnotation(const Annotation &ann)
{
    F0Contour *item = new F0Contour(ann);
    _scene.addItem(item);
    _displayed_anns.insert(ann, item);
}

F0Contour *F0Display::selected()
{
    foreach (F0Contour *contour, _displayed_anns) {
        if (contour->isHighlighted())
            return contour;
    }
    return 0;
}

void F0Display::refresh()
{
    if (isVisible()) {
        QHashIterator<Annotation,F0Contour*> it(_displayed_anns);
        while (it.hasNext()) {
            it.next();
            Annotation ann = it.key();
            if (ann.getF0() == 0 && !ann.getAudioPath().isEmpty()) {
                it.value()->computeF0();
            }
        }
    }
}

void F0Display::select(const Annotation &ann)
{
    QHashIterator<Annotation,F0Contour*> it(_displayed_anns);
    while (it.hasNext()) {
        it.next();
        if (it.key() == ann) {
            it.value()->setHighlighted(true);
            emit selectionChanged(ann);
        }
        else {
            it.value()->setHighlighted(false);
        }
    }
}

void F0Display::remove(const Annotation &ann)
{
    QHash<Annotation,F0Contour*>::iterator it =
            _displayed_anns.find(ann);
    if (it != _displayed_anns.end()) {
        _scene.removeItem(it.value());
        _displayed_anns.remove(ann);
    }
}

void F0Display::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    QTransform trans;
    trans.scale(viewport()->width() / 300.0, 1.0);
    setTransform(trans);
}

void F0Display::contextMenuEvent(QContextMenuEvent *event)
{
    QHashIterator<Annotation,F0Contour*> it(_displayed_anns);
    while (it.hasNext()) {
        it.next();
        if (it.value()->contains(mapToScene(event->pos()))) {
            Annotation ann = it.key();
            PlayMenu m(ann, this);
            m.exec(mapToGlobal(event->pos()));
            break;
        }
    }
}

void F0Display::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QHashIterator<Annotation,F0Contour*> it(_displayed_anns);
        while (it.hasNext()) {
            it.next();
            if (it.value()->contains(mapToScene(event->pos()))) {
                select(it.key());
                break;
            }
        }
    }
}

void F0Display::mouseDoubleClickEvent(QMouseEvent *event)
{
    QHashIterator<Annotation,F0Contour*> it(_displayed_anns);
    while (it.hasNext()) {
        it.next();
        if (it.value()->contains(mapToScene(event->pos()))) {
            select(it.key());
            Annotation ann(it.key());  // because it.key() is const
            ann.playTarget();
            break;
        }
    }
}

void F0Display::showEvent(QShowEvent *event)
{
    refresh();
    QGraphicsView::showEvent(event);
}
