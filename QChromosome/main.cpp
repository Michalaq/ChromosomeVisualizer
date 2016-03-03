#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    QSurfaceFormat::setDefaultFormat(format);

    QObject::connect(&a, &QApplication::focusChanged, [](QObject* old, QObject* now)
    {
        for (auto object : { old, now })
            if (auto spinBox = qobject_cast<QSpinBox*>(object))
            {
                spinBox->style()->unpolish(spinBox);
                spinBox->style()->polish(spinBox);
                spinBox->update();
            }
    });

    MainWindow w;
    w.showMaximized();

    a.installEventFilter(&w);

    return a.exec();
}
