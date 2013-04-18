#ifndef TARGETLIST_H
#define TARGETLIST_H

#include <QtGui>
#include <QtWidgets>
#include <annotation.h>

/// TargetList is a display widget for target labels.
class TargetList : public QListWidget
{
    Q_OBJECT

public:
    TargetList(QWidget *parent=0);
    ~TargetList();
    Annotation annotation(int i);
    Annotation annotation(const QListWidgetItem *item);
    int index(const Annotation &ann);
    void removeAnnotation(const Annotation &ann);

signals:
    void itemRemoved(const Annotation &ann);

public slots:

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

private:
    QPoint _drag_start_pos;
};

#endif // TARGETLIST_H
