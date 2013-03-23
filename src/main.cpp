#include <QtGui/QApplication>
#include "mainwindow.h"
#include "RInside.h"
#include "Rcpp.h"

#include "rinstance.h"

RInside *R;

int main(int argc, char *argv[])
{

    R = new RInside(argc, argv);

    qRegisterMetaTypeStreamOperators<Annotation>("Annotation");

    QApplication a(argc, argv);

    QCoreApplication::addLibraryPath(QApplication::applicationDirPath() +
                                     QDir::separator() +
                                     "plugins");

    MainWindow w;
    w.show();

    return a.exec();
}
