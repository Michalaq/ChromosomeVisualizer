#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setOrganizationName("Organizacja");
    QApplication::setApplicationName("Nazwa");

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    QSurfaceFormat::setDefaultFormat(format);

    QFontDatabase::addApplicationFont(":/fonts/DroidSans");
    QFontDatabase::addApplicationFont(":/fonts/DroidSans-Bold");

    MainWindow w;

    a.installEventFilter(&w);

    w.showMaximized();

    return a.exec();
}
