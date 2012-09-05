#include "cluster.h"
#include "ui_cluster.h"
#include "playmenu.h"
#include "clusterbox.h"
#include "com.h"

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

int Cluster::_hsvh = -1;
int Cluster::_hsvs = 255;
int Cluster::_hsvv = 200;

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

    changeColor(); // currently no color is assigned

    // if "Cluster1" exists, it will find a unique label
    setLabel("Cluster1");
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
    // make sure the label is unique
    // make it unique if necessary
    QString new_label = _mk_unique(label);
    ui->lineEdit->setText(new_label);
    for (int i=0; i < ui->listWidget->count(); ++i) {
        ui->listWidget->annotation(i).setTone(new_label);
    }
}

QString Cluster::getLabel() const
{
    return ui->lineEdit->text();
}

void Cluster::setColor(const QColor &color)
{
    QPalette p = ui->colorPanel->palette();
    p.setColor(QPalette::Window, color);
    ui->colorPanel->setPalette(p);
    _color = color;
}

QColor Cluster::getColor()
{
    return _color;
}

void Cluster::changeColor()
{
    _next_color();
    setColor(_color);
}

void Cluster::doColoring()
{
    for (int i=0; i < ui->listWidget->count(); ++i) {
        QString s = ui->listWidget->annotation(i).getTone2();
        if (!s.isEmpty() && s != getLabel()) {
            QColor c = COM.getColor(s);
            ui->listWidget->item(i)->setBackgroundColor(c);
        }
    }
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

void Cluster::on_lineEdit_editingFinished()
{
    setLabel(getLabel());
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

void Cluster::_next_color()
{
    if (_hsvh < 0) {
        _hsvh = rand() % 360;
        _hsvs = rand() % 196 + 60;
        _hsvv = rand() % 136 + 120;
    }

    _hsvh += 107;
    if (_hsvh >= 360) {
        _hsvh %= 360;
        _hsvs = _hsvs < 147 ? _hsvs - 147 + 256 : _hsvs - 89;
        _hsvv = _hsvv < 177 ? _hsvv - 177 + 256 : _hsvv - 59;
    }

    _color.setHsv(_hsvh, _hsvs, _hsvv);
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

QString Cluster::_mk_unique(const QString &label)
{
    QSet<QString> labels;
    ClusterBox *cb = dynamic_cast<ClusterBox*>(parent());
    foreach (Cluster *cluster, cb->getClusters()) {
        if (cluster != this)
            labels << cluster->getLabel();
    }
    if (labels.find(label) == labels.end())
        return label;
    QRegExp rx("\\d+\\s*$");
    int i = rx.indexIn(label);
    QString prefix = i >= 0 ? label.left(i) : label;
    QString new_label(prefix + "1");
    int c = 1;
    while (labels.find(new_label) != labels.end())
        new_label = prefix + QString("%1").arg(++c);
    return new_label;
}
