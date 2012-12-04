#include "playmenu.h"
#include <QDebug>

PlayMenu::PlayMenu(const Annotation &ann, QWidget *parent) :
    QMenu(parent),
    _ann(ann)
{
    _act_play_target = addAction("Play target");
    _act_play_frame = addAction("Play frame");
    _act_play_melody = addAction("Melody");

    if (ann.getFrameLabel().trimmed().isEmpty())
        _act_play_frame->setEnabled(false);

    connect(this, SIGNAL(triggered(QAction*)), SLOT(on_triggered(QAction*)));
}

void PlayMenu::on_triggered(QAction *action)
{
    if (action->text() == "Play target") {
        _ann.playTarget();
    }
    else if (action->text() == "Play frame") {
        if (!_ann.getFrameLabel().trimmed().isEmpty())
            _ann.playFrame();
    }
    else if (action->text() == "Melody") {
        _ann.hum();
    }
}
