#include <QtWidgets>

#include "VizWidget.hpp"
#include "DUPA.hpp"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setVersion(3, 3);
	format.setProfile(QSurfaceFormat::CoreProfile);
	QSurfaceFormat::setDefaultFormat(format);

	VizWidget viz;
	viz.show();

	QTimer * timer = new QTimer(&viz);
	DUPA d;
	d.frame = 0;
	d.v = &viz;
	QObject::connect(timer, SIGNAL(timeout()), &d, SLOT(onTick()));
	timer->start(16);

	return app.exec();
}
