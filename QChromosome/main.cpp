#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>
#include <QFontDatabase>
#include <QSplashScreen>

#include "commondata.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSplashScreen s(QPixmap(":/application/splash"), Qt::WindowStaysOnTopHint);
    s.show();

    QApplication::setOrganizationName("Regulomics");
    QApplication::setApplicationName("QChromosome 4D Studio");
    QApplication::setWindowIcon(QIcon(":/application/icon"));

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setVersion(4, 2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    QFontDatabase::addApplicationFont(":/fonts/Roboto-Regular");
    QFontDatabase::addApplicationFont(":/fonts/Roboto-Bold");
    QFontDatabase::addApplicationFont(":/fonts/RobotoMono-Regular");

    MainWindow w;

    a.installEventFilter(&w);

    w.showMaximized();

    s.finish(&w);

    int ans = a.exec();

    delete CommonData::getInstance();
    delete MaterialBrowser::getInstance();
    delete RenderSettings::getInstance();

    return ans;
}
