#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>
#include "filterobject.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    QSurfaceFormat::setDefaultFormat(format);

    MainWindow w;

    FilterObject f(&w);
    a.installEventFilter(&f);

    w.showMaximized();

    return a.exec();
}
