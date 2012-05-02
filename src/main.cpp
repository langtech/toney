#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    qRegisterMetaTypeStreamOperators<Annotation>("Annotation");

    QApplication a(argc, argv);

    QCoreApplication::addLibraryPath(QApplication::applicationDirPath() +
                                     QDir::separator() +
                                     "plugins");

    MainWindow w;
    w.show();

    return a.exec();
}
