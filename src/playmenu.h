#ifndef PLAYMENU_H
#define PLAYMENU_H

#include <QMenu>
#include <annotation.h>

class PlayMenu : public QMenu
{
    Q_OBJECT
public:
    explicit PlayMenu(const Annotation &ann, QWidget *parent = 0);

signals:

public slots:
    void on_triggered(QAction *action);

private:
    Annotation _ann;

};

#endif // PLAYMENU_H
