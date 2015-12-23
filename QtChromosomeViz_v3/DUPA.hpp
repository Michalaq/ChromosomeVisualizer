#pragma once

#include <QtWidgets>

#include "VizWidget.hpp"

class DUPA : public QObject
{
	Q_OBJECT;

public:
	frameNumber_t frame;
	VizWidget * v;

public slots:
	void onTick()
	{
		frame++;
		v->setFrame(frame);
		v->repaint();
	}
};
