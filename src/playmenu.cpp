#include "playmenu.h"
#include <QDebug>

PlayMenu::PlayMenu(const Annotation &ann, QWidget *parent) :
    QMenu(parent),
    _ann(ann)
{
    addAction("Play target");
    addAction("Play frame");
    addAction("Melody");
    connect(this, SIGNAL(triggered(QAction*)), SLOT(on_triggered(QAction*)));
}

void PlayMenu::on_triggered(QAction *action)
{
    if (action->text() == "Play target") {
        _ann.playTarget();
    }
    else if (action->text() == "Play frame") {
        _ann.playFrame();
    }
    else if (action->text() == "Melody") {
        _ann.hum();
    }
}
