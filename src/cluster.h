#ifndef CLUSTER_H
#define CLUSTER_H

#include <QtGui>
#include <annotationset.h>
#include <listplayer.h>
#include <config.h>

namespace Ui {
    class Cluster;
}

/// Cluster represents a group of Annotations with the same tone.
class Cluster : public QWidget
{
    Q_OBJECT

public:
    explicit Cluster(QWidget *parent = 0);
    ~Cluster();
    void addAnnotation(const Annotation &ann);
    void removeAnnotation(const Annotation &ann);
    void setLabel(const QString &label);
    QString getLabel() const;
    void setColor(const QColor &color);
    QColor getColor();
    void changeColor();
    void setConfig(const Config &config);
    QSet<Annotation> annotations() const;
    int getMaxLabelWidth();
    void fitToLabelWidth();

signals:
    void clusterRemovalRequested(Cluster *cluster);

public slots:
    void refreshF0Contour();

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

private slots:
    void on_lineEdit_textChanged(const QString &text);
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_listWidget_currentItemChanged(QListWidgetItem * current,
                                          QListWidgetItem * previous);
    void on_listWidget_itemRemoved(const Annotation &ann);
    void on_f0Display_selectionChanged(const Annotation &ann);
    void on_btnPlay_clicked();
    void on_btnPlay2_clicked();
    void on_btnStop_clicked();
    void on_btnDeleteCluster_clicked();
    void _finished_playing();

private:
    void _next_color();
    QString _item_label(const Annotation &ann);

private:
    static int _hsvh;
    static int _hsvs;
    static int _hsvv;

private:
    Ui::Cluster *ui;
    ListPlayer _list_player;
    Config _config;
    QPoint _drag_start_pos;
    QColor _color;
};

#endif // CLUSTER_H
