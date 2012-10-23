#include "clusterbox.h"
#include "ui_clusterbox.h"
#include "com.h"

ClusterBox::ClusterBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClusterBox)
{
    ui->setupUi(this);
    COM.registerClusterBox(this);
}

ClusterBox::~ClusterBox()
{
    delete ui;
}

Cluster *ClusterBox::getCluster(const QString &label)
{
    for (int i=1; i < ui->horizontalLayout->count() - 1; ++i) {
        QWidget *w = ui->horizontalLayout->itemAt(i)->widget();
        Cluster *c = dynamic_cast<Cluster*>(w);
        if (c->getLabel() == label) {
            return c;
        }
    }
    return 0;
}

QList<Cluster*> ClusterBox::getClusters()
{
    QList<Cluster*> l;
    for (int i=1; i < ui->horizontalLayout->count() - 1; ++i) {
        QWidget *w = ui->horizontalLayout->itemAt(i)->widget();
        Cluster *c = dynamic_cast<Cluster*>(w);
        l.append(c);
    }
    return l;
}

void ClusterBox::removeCluster(Cluster *cluster)
{
    ui->horizontalLayout->removeWidget(cluster);
    delete cluster;
}

int ClusterBox::clusterIndex(QWidget *widget)
{
    return ui->horizontalLayout->indexOf(widget);
}

void ClusterBox::setConfig(const Config &config)
{
    for (int i=1; i < ui->horizontalLayout->count() - 1; ++i) {
        QWidget *w = ui->horizontalLayout->itemAt(i)->widget();
        Cluster *c = dynamic_cast<Cluster*>(w);
        c->setConfig(config);
    }
}

void ClusterBox::fitColumnWidth()
{
    for (int i=1; i < ui->horizontalLayout->count() - 1; ++i) {
        QWidget *w = ui->horizontalLayout->itemAt(i)->widget();
        Cluster *c = dynamic_cast<Cluster*>(w);
        c->fitToLabelWidth();
    }
}

Cluster *ClusterBox::addCluster()
{
    Cluster *c = new Cluster(this);
    int idx = ui->horizontalLayout->count() - 1;
    ui->horizontalLayout->insertWidget(idx, c);
    return c;
}

Cluster *ClusterBox::addCluster(const QString& label)
{
    Cluster *c = addCluster();
    c->setLabel(label);
    return c;
}

void ClusterBox::clear(const AnnotationSet *pool)
{
    for (int i=1; i < ui->horizontalLayout->count() - 1; ++i) {
        QWidget *w = ui->horizontalLayout->itemAt(i)->widget();
        Cluster *cluster = dynamic_cast<Cluster*>(w);
        foreach (const Annotation &ann, pool->getAnnotations()) {
            cluster->removeAnnotation(ann);
        }
    }
}

void ClusterBox::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void ClusterBox::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void ClusterBox::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("Cluster")) {
        QPoint pos = event->pos();
        QWidget *w = this->childAt(pos);
        if (w == 0)  // can't drop on top of other cluster
            event->acceptProposedAction();
    }
}

void ClusterBox::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("Cluster")) {
        int curidx = event->mimeData()->data("Cluster").toInt();
        int idx = ui->horizontalLayout->count() - 1;
        for (int i=1; i < ui->horizontalLayout->count()-1; ++i) {
            // leftmost and rightmost widget are spacers
            QLayoutItem *item = ui->horizontalLayout->itemAt(i);
            if (event->pos().x() < item->geometry().left()) {
                idx = i;
                break;
            }
        }
        if (!((curidx == idx) ||      // dropped in a spece on the left edge
              (curidx == idx - 1))) { // dropped in a space on thr right edge
            QLayoutItem *item = ui->horizontalLayout->takeAt(curidx);
            if (item) {
                if (curidx < idx)
                    ui->horizontalLayout->insertWidget(idx-1, item->widget());
                else
                    ui->horizontalLayout->insertWidget(idx, item->widget());
                delete item;
            }
        }
    }
}
