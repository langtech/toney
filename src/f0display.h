#ifndef F0DISPLAY_H
#define F0DISPLAY_H

#include <QGraphicsView>
#include <targetlist.h>
#include <QHash>
#include <f0contour.h>

class F0Display : public QGraphicsView
{
    Q_OBJECT
public:
    explicit F0Display(QWidget *parent = 0);
    ~F0Display();
    void addAnnotation(const Annotation &ann);
    F0Contour *selected();
    void refresh();

signals:
    void selectionChanged(const Annotation &ann);
    
public slots:
    void select(const Annotation &ann);
    void remove(const Annotation &ann);

protected:
    void resizeEvent(QResizeEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void showEvent(QShowEvent *event);

private:
    QGraphicsScene _scene;
    QHash<Annotation,F0Contour*> _displayed_anns;
};

#endif // F0DISPLAY_H
