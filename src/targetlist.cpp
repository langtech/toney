#include "targetlist.h"
#include "playmenu.h"

TargetList::TargetList(QWidget *parent)
    :QListWidget(parent)
{
}

TargetList::~TargetList()
{
}

Annotation TargetList::annotation(int i)
{
    return annotation(item(i));
}

Annotation TargetList::annotation(const QListWidgetItem *item)
{
    QVariant v = item->data(Qt::UserRole);
    return Annotation(v.value<Annotation>());
}

int TargetList::index(const Annotation &ann)
{
    for (int i=0; i < count(); ++i) {
        if (ann == annotation(i))
            return i;
    }
    return -1;
}

void TargetList::removeAnnotation(const Annotation &ann)
{
    int idx = index(ann);
    if (idx >= 0) {
        QListWidgetItem *item = this->takeItem(idx);
        delete item;
        emit itemRemoved(ann);
    }
}

void TargetList::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        _drag_start_pos = event->pos();

    QListWidget::mousePressEvent(event);
}

void TargetList::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton)) {
        QListWidget::mouseMoveEvent(event);
        return;
    }
    if ((event->pos() - _drag_start_pos).manhattanLength()
            < QApplication::startDragDistance()) {
        QListWidget::mouseMoveEvent(event);
        return;
    }

    QListWidgetItem *item = itemAt(event->pos());
    if (item != 0) {
        QMimeData *data = new QMimeData;
        data->setData("Annotation", annotation(item).getId());
        QDrag *drag = new QDrag(this);
        drag->setMimeData(data);
        Annotation ann(this->annotation(item));
        switch (drag->exec()) {
        case Qt::MoveAction:
            removeAnnotation(ann);
            break;
        default:
            break;
        }
    }
}

void TargetList::contextMenuEvent(QContextMenuEvent *event)
{
    QListWidgetItem *item = itemAt(event->pos());
    if (item != 0) {
        PlayMenu m(annotation(item), this);
        m.exec(mapToGlobal(event->pos()));
    }
}
