#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui>
#include "textgrid.h"
#include "toney_utils.h"
#include <sndfile.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _player(this)
{
    ui->setupUi(this);

    COM.registerGetF0ParamsDialog(&_f0dialog);
    COM.registerGetValuePositionDialog(&_valposdialog);

    connect(&_f0dialog, SIGNAL(accepted()), SLOT(redoGetF0()));
    connect(&_valposdialog, SIGNAL(accepted()), SLOT(redoClusters()));
}

MainWindow::~MainWindow()
{
    delete ui;
    QHash<QString,AnnotationSet*>::iterator i;
    for (i = _pools.begin(); i != _pools.end(); ++i) {
        delete i.value();
    }
    _pools.clear();
}

void MainWindow::on_action_Open_triggered()
{
    QString filename = QFileDialog::getOpenFileName(
                this,
                "Open TextGrid",
                QString(),
                "*.TextGrid");
    if (filename.isNull())
        return;

    _open_textgrid(filename);
}

void MainWindow::on_action_Open_Directory_triggered()
{
    QString dirname = QFileDialog::getExistingDirectory(this, "Open Directory");
    if (dirname.isEmpty())
        return;

    QDir dir(dirname);
    QStringList filter("*.TextGrid");
    foreach (QString filename, dir.entryList(filter)) {
        _open_textgrid(dir.filePath(filename));
    }
}

void MainWindow::on_action_Save_triggered()
{
    QHashIterator<QString,AnnotationSet*> it(_pools);
    while (it.hasNext()) {
        it.next();
        AnnotationSet *pool = it.value();
        if (pool->save() == false)
            _show_save_error(it.key());
    }
}

void MainWindow::on_action_Add_cluster_triggered()
{
    Cluster *c = ui->scrollAreaWidgetContents->addCluster();
    connect(c, SIGNAL(clusterRemovalRequested(Cluster*)),
            SLOT(removeCluster(Cluster*)));
}

void MainWindow::on_action_Reclassify_triggered()
{
    QHash<const Annotation, QString> s;
    foreach (AnnotationSet* aset, _pools.values()) {
        foreach (Annotation ann, aset->getAnnotations()) {
            s.insert(ann, "");
        }
    }

    int pos = COM.valuePosition();
    reclassify(s, pos);

    QHash<const Annotation, QString>::iterator i = s.begin();
    for (; i != s.end(); ++i) {
        Annotation ann = i.key();
        if (ann.getValue(pos) != i.value())
            ann.setValue2(pos, i.value());
    }

    ui->poolWidget->doColoring();

    foreach (Cluster *cluster, ui->scrollAreaWidgetContents->getClusters()) {
        cluster->doColoring();
    }
}

void MainWindow::on_action_F0_Params_triggered()
{
    _f0dialog.show();
}

void MainWindow::on_action_Value_Position_triggered()
{
    _old_pos = _valposdialog.getPosition();
    _valposdialog.show();
}

void MainWindow::on_action_Exit_triggered()
{
    close();
}

void MainWindow::on_action_Show_Frame_ID_toggled(bool v)
{
    _config.show_frame_label = v;
    ui->scrollAreaWidgetContents->setConfig(_config);
    ui->poolWidget->setConfig(_config);
}

void MainWindow::on_action_Show_Speaker_ID_toggled(bool v)
{
    _config.show_speaker_label = v;
    ui->scrollAreaWidgetContents->setConfig(_config);
    ui->poolWidget->setConfig(_config);
}

void MainWindow::on_action_Resize_Column_Width_triggered()
{
    ui->scrollAreaWidgetContents->fitColumnWidth();
}

void MainWindow::on_action_Playback_Delay_toggled(bool v)
{
    _config.playback_delay = v;
    ui->scrollAreaWidgetContents->setConfig(_config);
}

void MainWindow::on_action_help_triggered(bool v)
{
    Q_UNUSED(v);
    _help.show();
}

void MainWindow::on_volumeSlider_valueChanged(int v)
{
    Player::setVolumeLevel(v);
}

void MainWindow::removeCluster(Cluster *cluster)
{
    QString s("Do you really want to remove this column?\n\n[%1]");

    QMessageBox::StandardButton ans = QMessageBox::question(
                this,
                "Delete cluster",
                s.arg(cluster->getLabel()),
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No);

    if (ans == QMessageBox::Yes) {
        int pos = COM.valuePosition();
        foreach (const Annotation &ann, cluster->annotations()) {
            Annotation a(ann);
            a.clearValue(pos);
            ui->poolWidget->addAnnotation(a);
        }

        delete cluster;
    }
}

void MainWindow::redoGetF0()
{
    if (_f0dialog.paramsChanged()) {
        foreach (AnnotationSet *aset, _pools) {
            foreach (Annotation ann, aset->getAnnotations()) {
                ann.clearPitch();
            }
        }
        foreach (Cluster *c, ui->scrollAreaWidgetContents->getClusters()) {
            c->refreshF0Contour();
        }
    }
}

void MainWindow::redoClusters()
{
    int pos = _valposdialog.getPosition();
    if (pos != _old_pos) {
        ClusterBox *cbox = ui->scrollAreaWidgetContents;
        foreach (Cluster *cluster, cbox->getClusters())
            cbox->removeCluster(cluster);
        foreach (AnnotationSet *pool, _pools.values()) {
            ui->poolWidget->clear(pool);
            foreach (const Annotation &ann, pool->getAnnotations()) {
                QString cluster_label = ann.getValue(pos);
                if (cluster_label.isEmpty())
                    ui->poolWidget->addAnnotation(ann);
                else {
                    Cluster *c = cbox->getCluster(cluster_label);
                    if (c == 0) {
                        c = cbox->addCluster(cluster_label);
                        connect(c, SIGNAL(clusterRemovalRequested(Cluster*)),
                                SLOT(removeCluster(Cluster*)));
                    }
                    c->addAnnotation(ann);
                }
            }
        }
    }
}

QString MainWindow::findAudio(const QString &textgrid_filename)
{
    QFileInfo fi(textgrid_filename);
    QString stem = fi.baseName().replace("\\.TextGrid$", "");
    QDir dir = fi.dir();
    QStringList filters;
    filters << stem + ".*";
    dir.setNameFilters(filters);
    QStringList files = dir.entryList(QDir::Files | QDir::Readable);
    QStringList audio_files;
    for (int i=0; i < files.size(); ++i) {
        if (isAudio(dir.filePath(files.at(i)))) {
            audio_files << files.at(i);
        }
    }

    if (audio_files.size() > 1) {
        bool ok;
        QString audio_file = QInputDialog::getItem(
                    this,
                    "Select audio file",
                    "Multiple audio files detected. Please select \n"
                    "the audio file you want to open.\n\n"
                    "TextGrid: " + stem,
                    audio_files,
                    0,
                    false,
                    &ok);
        if (ok)
            return dir.filePath(audio_file);
        else
            return QString();
    }
    else if (audio_files.size() > 0) {
        return dir.filePath(audio_files.at(0));
    }
    else {
        return QString();
    }
}

bool MainWindow::isAudio(const QString &filename)
{
    SF_INFO sfinfo;
    SNDFILE *sndfile = sf_open(
                filename.toUtf8().constData(), SFM_READ, &sfinfo);
    if (sndfile == 0)
        return false;

    sf_close(sndfile);
    return true;
}

// This should be called only when the annotation file is newly loaded or
// when the audio file is newly loaded.
void MainWindow::_set_audio_path(AnnotationSet *pool, const QString &audio_path)
{
    foreach (const Annotation &ann, pool->getAnnotations()) {
        Annotation a(ann);
        a.setAudioPath(audio_path);
    }

    QLayout *layout = ui->scrollAreaWidgetContents->layout();
    for (int i=0; i < layout->count(); ++i) {
        QLayoutItem *item = layout->itemAt(i);
        if (item->widget()  && item->widget()->inherits("Cluster")) {
            Cluster *cluster = dynamic_cast<Cluster*>(item->widget());
            cluster->refreshF0Contour();
        }
    }
}

void MainWindow::_show_save_error(const QString &filename)
{
    QMessageBox::critical(
                this,
                "Save error",
                "Failed to save the file:\n\n" + filename
                );
}

void MainWindow::_open_textgrid(const QString &filename)
{
    if (_pools.contains(filename)) {
        AnnotationSet *pool = _pools.value(filename);
        if (pool->modified()) {
            QMessageBox::StandardButton ans = QMessageBox::question(
                        this,
                        "Save changes",
                        "There are changes that haven't been saved.\n"
                        "Do you want to save them before re-loading the file?",
                        QMessageBox::Yes | QMessageBox::No,
                        QMessageBox::Yes);
            if (ans == QMessageBox::Yes) {
                if (pool->save() != true)
                    _show_save_error(filename);
            }
        }
        ui->poolWidget->clear(pool);
        ui->scrollAreaWidgetContents->clear(pool);
        _pools.remove(filename);
        delete pool;
    }

    AnnotationSet *pool = new TextGrid;
    _pools.insert(filename, pool);

    if (!pool->open(filename)) {
        QMessageBox::critical(
                    this,
                    "File open error",
                    "Failed to open the file.\n"
                    "Please check whether the file exists and\n"
                    "is well formatted."
                    );
        _pools.remove(filename);
        delete pool;
        return;
    }

    int pos = COM.valuePosition();

    foreach (const Annotation &ann, pool->getAnnotations()) {
        if (ann.getValue(pos).isEmpty())
            ui->poolWidget->addAnnotation(ann);
        else {
            QString tone = ann.getValue(pos);
            Cluster *c = ui->scrollAreaWidgetContents->getCluster(tone);
            if (c == 0) {
                c = ui->scrollAreaWidgetContents->addCluster(tone);
                connect(c, SIGNAL(clusterRemovalRequested(Cluster*)),
                        SLOT(removeCluster(Cluster*)));
            }
            c->addAnnotation(ann);
        }
    }

    QString audio_path = findAudio(filename);
    if (!audio_path.isEmpty())
        _set_audio_path(pool, audio_path);

    ui->action_Save->setEnabled(true);
}
