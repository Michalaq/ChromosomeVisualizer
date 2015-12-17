#include <QtWidgets>

#include "mainwidget.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setVersion(3, 3);
	format.setProfile(QSurfaceFormat::CoreProfile);
	QSurfaceFormat::setDefaultFormat(format);

    /*VizWidget viz;
    viz.show();*/

    MainWidget wid;
    wid.show();

	return app.exec();
}
