#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>
#include <QFontDatabase>
#include <QSplashScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSplashScreen s(QPixmap(":/application/splash").scaled(600, 500), Qt::WindowStaysOnTopHint);
    s.show();

    QApplication::setOrganizationName("Regulomics");
    QApplication::setApplicationName("QChromosome 4D Studio");
    QApplication::setWindowIcon(QIcon(":/application/icon"));

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

    s.finish(&w);

    return a.exec();
}
