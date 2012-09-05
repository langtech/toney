#ifndef CLUSTERBOX_H
#define CLUSTERBOX_H

#include <QWidget>
#include <annotationset.h>
#include <cluster.h>
#include <config.h>

namespace Ui {
    class ClusterBox;
}

/// ClusterBox is a container for Cluster objects.
class ClusterBox : public QWidget
{
    Q_OBJECT

public:
    explicit ClusterBox(QWidget *parent = 0);
    ~ClusterBox();

    Cluster *getCluster(const QString& label);
    QList<Cluster*> getClusters();
    void removeCluster(Cluster *cluster);
    int clusterIndex(QWidget *widget);
    void setConfig(const Config &config);
    void fitColumnWidth();

public slots:
    Cluster *addCluster();
    Cluster *addCluster(const QString& label);
    void clear(const AnnotationSet *pool);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);

private:
    Ui::ClusterBox *ui;
};

#endif // CLUSTERBOX_H
