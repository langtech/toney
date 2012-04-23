#include "cluster.h"
#include "ui_cluster.h"
#include "playmenu.h"
#include "clusterbox.h"

static const char * const play_frame_xpm[] = {
    "16 16 2 1",
    ". c None",
    "x c #00aa00",
    "................",
    "................",
    "................",
    "...xx...........",
    "...xxxx.........",
    "...xxxxxx.......",
    "...xxxxxxxx.....",
    "...xxxxxxxxxx...",
    "...xxxxxxxxxx...",
    "...xxxxxxxx.... ",
    "...xxxxxx.......",
    "...xxxx.........",
    "...xx...........",
    "................",
    "................",
    "................",
};

static const char * const play_frame_xpm2[] = {
    "16 16 2 1",
    ". c None",
    "x c #004400",
    "................",
    "................",
    "................",
    "...xx...........",
    "...xxxx.........",
    "...xxxxxx.......",
    "...xxxxxxxx.....",
    "...xxxxxxxxxx...",
    "...xxxxxxxxxx...",
    "...xxxxxxxx.... ",
    "...xxxxxx.......",
    "...xxxx.........",
    "...xx...........",
    "................",
    "................",
    "................",
};

static const char * const play_tone_xpm[] = {
    "16 16 2 1",
    ". c None",
    "x c #008800",
    "................",
    "................",
    "................",
    "................",
    "................",
    ".....xx.........",
    ".....xxxx.......",
    ".....xxxxxx.....",
    ".....xxxxxx.....",
    ".....xxxx...... ",
    ".....xx.........",
    "................",
    "................",
    "................",
    "................",
    "................",
};

static const char * const play_tone_xpm2[] = {
    "16 16 2 1",
    ". c None",
    "x c #004400",
    "................",
    "................",
    "................",
    "................",
    "................",
    ".....xx.........",
    ".....xxxx.......",
    ".....xxxxxx.....",
    ".....xxxxxx.....",
    ".....xxxx...... ",
    ".....xx.........",
    "................",
    "................",
    "................",
    "................",
    "................",
};

static const char * const pause_xpm[] = {
    "16 16 2 1",
    ". c None",
    "x c #000000",
    "................",
    "................",
    "................",
    "...xxx....xxx...",
    "...xxx....xxx...",
    "...xxx....xxx...",
    "...xxx....xxx...",
    "...xxx....xxx...",
    "...xxx....xxx...",
    "...xxx....xxx.. ",
    "...xxx....xxx...",
    "...xxx....xxx...",
    "...xxx....xxx...",
    "................",
    "................",
    "................",
};

static const char * const stop_xpm[] = {
    "16 16 2 1",
    ". c None",
    "x c #aa0000",
    "................",
    "................",
    "................",
    "...xxxxxxxxxx...",
    "...xxxxxxxxxx...",
    "...xxxxxxxxxx...",
    "...xxxxxxxxxx...",
    "...xxxxxxxxxx...",
    "...xxxxxxxxxx...",
    "...xxxxxxxxxx.. ",
    "...xxxxxxxxxx...",
    "...xxxxxxxxxx...",
    "...xxxxxxxxxx...",
    "................",
    "................",
    "................",
};

static const char * const delete_cluster_xpm[] = {
    "16 16 2 1",
    ". c None",
    "x c #aa0000",
    "................",
    "................",
    "................",
    "................",
    "....xx....xx....",
    "....xxx..xxx....",
    ".....xxxxxx.....",
    "......xxxx......",
    "......xxxx......",
    ".....xxxxxx.... ",
    "....xxx..xxx....",
    "....xx....xx....",
    "................",
    "................",
    "................",
    "................",
};


Cluster::Cluster(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Cluster),
    _list_player(this)
{
    ui->setupUi(this);
    ui->btnPlay->setIcon(QIcon(QPixmap(play_tone_xpm)));
    ui->btnPlay2->setIcon(QIcon(QPixmap(play_frame_xpm)));
    ui->btnStop->setIcon(QIcon(QPixmap(stop_xpm)));
    ui->btnDeleteCluster->setIcon(QIcon(QPixmap(delete_cluster_xpm)));
    _list_player.setList(ui->listWidget);
    connect(&_list_player,
            SIGNAL(finishedPlaying()),
            SLOT(_finished_playing()));
}

Cluster::~Cluster()
{
    delete ui;
}

void Cluster::addAnnotation(const Annotation &ann)
{
    if (ann.getTone() != getLabel())
        Annotation(ann).setTone(ui->lineEdit->text());

    QListWidgetItem *item = new QListWidgetItem(_item_label(ann));
    item->setData(Qt::UserRole, QVariant::fromValue<Annotation>(ann));
    ui->listWidget->addItem(item);

    ui->f0Display->addAnnotation(ann);
}

void Cluster::removeAnnotation(const Annotation &ann)
{
    ui->listWidget->removeAnnotation(ann);
}

void Cluster::setLabel(const QString& label)
{
    ui->lineEdit->setText(label);
    for (int i=0; i < ui->listWidget->count(); ++i) {
        QListWidgetItem *item = ui->listWidget->item(i);
        QVariant data = item->data(Qt::UserRole);
        Annotation ann = data.value<Annotation>();
        ann.setTone(label);
    }
}

QString Cluster::getLabel() const
{
    return ui->lineEdit->text();
}

void Cluster::setConfig(const Config &config)
{
    if (config.show_frame_label != _config.show_frame_label ||
            config.show_speaker_label != _config.show_speaker_label) {
        _config.show_frame_label = config.show_frame_label;
        _config.show_speaker_label = config.show_speaker_label;
        for (int i=0; i < ui->listWidget->count(); ++i) {
            QListWidgetItem *item = ui->listWidget->item(i);
            Annotation ann = ui->listWidget->annotation(item);
            item->setText(_item_label(ann));
        }
    }

    _list_player.setConfig(config);
}

QSet<Annotation> Cluster::annotations() const
{
    QSet<Annotation> anns;
    for (int i=0; i < ui->listWidget->count(); ++i) {
        Annotation ann = ui->listWidget->annotation(i);
        anns.insert(ann);
    }
    return anns;
}

int Cluster::getMaxLabelWidth()
{
    QFontMetrics fm(ui->listWidget->font());
    int max = -1;
    for (int i=0; i < ui->listWidget->count(); ++i) {
        QListWidgetItem *item = ui->listWidget->item(i);
        int v = fm.width(item->text());
        if (v > max)
            max = v;
    }
    return max;
}

void Cluster::fitToLabelWidth()
{
    int max = getMaxLabelWidth();
    QMargins mar = ui->listWidget->contentsMargins();
    // account for spaces taken up by decoration
    max += 2 * (ui->listWidget->spacing() +
                ui->listWidget->lineWidth() +
                mar.left() + mar.right());
    ui->listWidget->setMinimumWidth(max);
}

void Cluster::refreshF0Contour()
{
    ui->f0Display->refresh();
}

void Cluster::dragEnterEvent(QDragEnterEvent *event)
{
    QRect r = ui->listWidget->rect();
    QRect t = event->answerRect();
    if (event->mimeData()->hasFormat("Annotation") && r.contains(t)) {
        event->acceptProposedAction();
    }
}

void Cluster::dropEvent(QDropEvent *event)
{
    if (event->source() == 0) {
        event->setDropAction(Qt::IgnoreAction);
    }
    else if (event->source() == this) {
        event->setDropAction(Qt::IgnoreAction);
    }
    else if (event->mimeData()->hasFormat("Annotation")) {
        QByteArray ba = event->mimeData()->data("Annotation");
        Annotation ann(ba);

        addAnnotation(ann);

        event->acceptProposedAction();
    }
}

void Cluster::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        _drag_start_pos = event->pos();

    QWidget::mousePressEvent(event);
}

void Cluster::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton)) {
        QWidget::mouseMoveEvent(event);
        return;
    }
    if ((event->pos() - _drag_start_pos).manhattanLength()
            < QApplication::startDragDistance()) {
        QWidget::mouseMoveEvent(event);
        return;
    }

    QMimeData *data = new QMimeData;
    QByteArray mimedata;
    // the index of cluster within the cluster box
    ClusterBox *cbox = dynamic_cast<ClusterBox*>(this->parentWidget());
    int idx = cbox->clusterIndex(this);
    mimedata.setNum(idx);
    data->setData("Cluster", mimedata);
    QDrag *drag = new QDrag(this);
    drag->setMimeData(data);
    switch (drag->exec()) {
    case Qt::MoveAction:
        break;
    default:
        break;
    }
}

void Cluster::on_lineEdit_textChanged(const QString &text)
{
    for (int i=0; i < ui->listWidget->count(); ++i) {
        ui->listWidget->annotation(i).setTone(text);
    }
}

void Cluster::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    ui->listWidget->annotation(item).playTarget();
}

void Cluster::on_listWidget_currentItemChanged(
        QListWidgetItem *current,
        QListWidgetItem *previous)
{
    Q_UNUSED(previous);
    if (current) {
        Annotation ann = ui->listWidget->annotation(current);
        ui->f0Display->select(ann);
    }
}

void Cluster::on_listWidget_itemRemoved(const Annotation &ann)
{
    ui->f0Display->remove(ann);
}

void Cluster::on_f0Display_selectionChanged(const Annotation &ann)
{
    int i = ui->listWidget->index(ann);
    ui->listWidget->setCurrentRow(i);
}

void Cluster::on_btnPlay_clicked()
{
    ListPlayer::STATE state = _list_player.state();

    if (_list_player.mode() != ListPlayer::TARGET &&
            state != ListPlayer::STOPPED) {
        return;
    }

    if (state == ListPlayer::STOPPED) {
        _list_player.setMode(ListPlayer::TARGET);
        _list_player.play();
        ui->btnPlay->setIcon(QIcon(QPixmap(pause_xpm)));
    }
    else if (state == ListPlayer::PLAYING) {
        _list_player.pause();
        ui->btnPlay->setIcon(QIcon(QPixmap(play_tone_xpm2)));
    }
    else if (state == ListPlayer::PAUSED) {
        _list_player.resume();
        ui->btnPlay->setIcon(QIcon(QPixmap(pause_xpm)));
    }
}

void Cluster::on_btnPlay2_clicked()
{
    ListPlayer::STATE state = _list_player.state();

    if (_list_player.mode() != ListPlayer::FRAME &&
            state != ListPlayer::STOPPED) {
        return;
    }

    if (state == ListPlayer::STOPPED) {
        _list_player.setMode(ListPlayer::FRAME);
        _list_player.play();
        ui->btnPlay2->setIcon(QIcon(QPixmap(pause_xpm)));
    }
    else if (state == ListPlayer::PLAYING) {
        _list_player.pause();
        ui->btnPlay2->setIcon(QIcon(QPixmap(play_frame_xpm2)));
    }
    else if (state == ListPlayer::PAUSED) {
        _list_player.resume();
        ui->btnPlay2->setIcon(QIcon(QPixmap(pause_xpm)));
    }
}

void Cluster::on_btnStop_clicked()
{
    _list_player.stop();
}

void Cluster::on_btnDeleteCluster_clicked()
{
    emit clusterRemovalRequested(this);
}

void Cluster::_finished_playing()
{
    if (_list_player.mode() == ListPlayer::TARGET) {
        ui->btnPlay->setIcon(QIcon(QPixmap(play_tone_xpm)));
    }
    else {
        ui->btnPlay2->setIcon(QIcon(QPixmap(play_frame_xpm)));
    }
}

QString Cluster::_item_label(const Annotation &ann)
{
    QString label = ann.getTargetLabel();
    if (_config.show_frame_label)
        label += "-" + ann.getFrameLabel();
    if (_config.show_speaker_label)
        label += "-" + ann.getSpeaker();
    return label;
}
