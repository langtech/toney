#include "pool.h"
#include "item.h"
#include "playmenu.h"

Pool::Pool(QWidget *parent) :
    QGraphicsView(parent),
    _drag_item(0)
{
    setScene(&_scene);
    setAcceptDrops(true);
}

Pool::~Pool()
{
    if (_drag_item)
        delete _drag_item;
}

void Pool::addAnnotation(const Annotation &ann)
{
    Item *item = new Item;
    // attach FileInfo
    item->setData(0, QVariant::fromValue<Annotation>(ann));
    item->setPlainText(_label(ann));
    _scene.addItem(item);

    int c = 0;
    while (c++ < 500) {
        // set random position
        qreal x = static_cast<qreal>(rand()) / RAND_MAX * _scene.width();
        qreal y = static_cast<qreal>(rand()) / RAND_MAX * _scene.height();
        item->setPos(x, y);

        // check collision
        QList<QGraphicsItem*> items = item->collidingItems();
        if (items.count() == 0)
            break;
    }

    _anns.insert(ann, item);
}

void Pool::setConfig(const Config &config)
{
    if (config.show_frame_label == _config.show_frame_label &&
            config.show_speaker_label == _config.show_speaker_label)
        return;

    _config = config;
    foreach (QGraphicsItem *item, _scene.items()) {
        Item *i = dynamic_cast<Item*>(item);
        i->setPlainText(_label(_item2ann(i)));
    }
}

void Pool::clear(const AnnotationSet *anns)
{
    foreach (Annotation ann, anns->getAnnotations()) {
        QGraphicsItem *item = _anns.value(ann, 0);
        if (item != 0) {
            _scene.removeItem(item);
            _anns.remove(ann);
            delete item;
        }
    }
}

void Pool::dragEnterEvent(QDragEnterEvent *event)
{
    Q_UNUSED(event);
    if (event->mimeData()->hasFormat("Annotation")) {
        event->acceptProposedAction();
    }
}

void Pool::dragLeaveEvent(QDragLeaveEvent *event)
{
    Q_UNUSED(event);
}

void Pool::dragMoveEvent(QDragMoveEvent *event)
{
    // this method does nothing but necessary to allow items moving
    // within the widget
    Q_UNUSED(event);
}

void Pool::dropEvent(QDropEvent *event)
{
    if (event->source() == 0) {
        event->setDropAction(Qt::IgnoreAction);
    }
    else if (event->source() == this) {
        event->setDropAction(Qt::IgnoreAction);
        _drag_item->setPos(mapToScene(event->pos()));
    }
    else if (event->mimeData()->hasFormat("Annotation")) {
        QByteArray ba = event->mimeData()->data("Annotation");
        Annotation ann(ba);

        ann.clearTone();

        Item *item = new Item;
        item->setData(0, QVariant::fromValue<Annotation>(ann));
        item->setPlainText(_label(ann));
        _scene.addItem(item);
        _anns.insert(ann, item);

        item->setPos(mapToScene(event->pos()));

        event->acceptProposedAction();
    }
}

void Pool::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        _drag_start_pos = event->pos();

    // relay the event
    QGraphicsView::mousePressEvent(event);
}

void Pool::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton)) {
        QGraphicsView::mouseMoveEvent(event);
        return;
    }
    if ((event->pos() - _drag_start_pos).manhattanLength()
            < QApplication::startDragDistance()) {
        QGraphicsView::mouseMoveEvent(event);
        return;
    }

    QPointF p = mapToScene(event->pos());
    _drag_item = dynamic_cast<Item*>(_scene.itemAt(p));
    if (_drag_item != 0) {
        _drag_item->hide();

        // serialize FileInfo obj associated with selected Item obj
        QVariant v = _drag_item->data(0);
        Annotation ann(v.value<Annotation>());

        QMimeData *data = new QMimeData;
        data->setData("Annotation", ann.getId());

        QDrag *drag = new QDrag(this);
        drag->setMimeData(data);
        drag->setPixmap(*_drag_item->screenshot());
        switch (drag->exec()) {
        case Qt::MoveAction:
            _scene.removeItem(_drag_item);
            _anns.remove(ann);
            delete _drag_item;
            break;
        case Qt::IgnoreAction:
            _drag_item->show();
            break;
        default:
            break;
        }
        _drag_item = 0;
    }

}

void Pool::contextMenuEvent(QContextMenuEvent *event)
{
    QGraphicsItem *item = itemAt(event->pos());
    if (item != 0) {
        PlayMenu m(_item2ann(item), this);
        m.exec(mapToGlobal(event->pos()));
    }
}

Annotation Pool::_item2ann(QGraphicsItem *item)
{
    QVariant v = item->data(0);
    return Annotation(v.value<Annotation>());
}

QString Pool::_label(const Annotation &ann)
{
    QString label = ann.getTargetLabel();
    if (_config.show_frame_label)
        label += "-" + ann.getFrameLabel();
    if (_config.show_speaker_label)
        label += "-" + ann.getSpeaker();
    return label;
}
