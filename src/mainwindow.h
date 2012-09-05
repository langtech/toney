#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "annotationset.h"
#include "pool.h"
#include "config.h"
#include "cluster.h"
#include "helpdialog.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void on_action_Open_triggered();
    void on_action_Open_Directory_triggered();
    void on_action_Save_triggered();
    void on_action_Add_cluster_triggered();
    void on_action_Reclassify_triggered();
    void on_action_Exit_triggered();
    void on_action_Show_Frame_ID_toggled(bool);
    void on_action_Show_Speaker_ID_toggled(bool);
    void on_action_Resize_Column_Width_triggered();
    void on_action_Playback_Delay_toggled(bool);
    void on_action_help_triggered(bool);
    void on_volumeSlider_valueChanged(int);

private slots:
    void removeCluster(Cluster *cluster);

private:
    QString findAudio(const QString &textgrid_filename);
    bool isAudio(const QString &filename);
    void _set_audio_path(AnnotationSet *pool, const QString &audio_path);
    void _show_save_error(const QString &filename);
    void _open_textgrid(const QString &filename);

private:
    Ui::MainWindow *ui;
    Config _config;
    QHash<QString,AnnotationSet*> _pools; // set of loaded annotation files
    HelpDialog _help;
#ifdef QTPLAYER
    QtPlayer _player;
#else
    SoxPlayer _player;
#endif
};

#endif // MAINWINDOW_H
