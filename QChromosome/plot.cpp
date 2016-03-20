#include "plot.h"
#include "../QtChromosomeViz_v2/bartekm_code/NullSimulation.h"

Plot::Plot(QWidget *parent) :
    QWidget(parent),
    simulation_(std::make_shared<NullSimulation>()),
    firstFrame(0),
    currentFrame(0),
    lastFrame(0)
{

}

Plot::~Plot()
{

}

void Plot::setSimulation(std::shared_ptr<Simulation> dp)
{
    simulation_ = std::move(dp);

    data.clear();

    firstFrame = 0;
    currentFrame = 0;
    lastFrame = 0;

    maxval = 0;

    update();
}

void Plot::setMinimum(int m)
{
    firstFrame = m;
    update();
}

void Plot::setMaximum(int m)
{
    for (int i = data.size(); i <= m; i++)
    {
        qreal y = simulation_->getFrame(i)->functionValues["bonds"];
        data << QPointF(i, y);

        if (maxval < y)
            maxval = y;
    }

    lastFrame = m;
    update();
}

void Plot::setFrame(int n)
{
    currentFrame = n;
    update();
}

#include <QPainter>

void Plot::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if (!simulation_ || data.isEmpty())
        return;

    QPainter painter(this);

    painter.setWindow(0, height(), width(), -height());
    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate(12, 12);
    painter.scale((qreal) (width() - 24) / (lastFrame - firstFrame), (qreal) (height() - 24) / maxval);
    painter.translate(-firstFrame, 0);

    //painter.fillPath(path, QColor("#002255"));

    QPen pen(QColor("#003380"), 2., Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    pen.setCosmetic(true);

    painter.setPen(pen);
    painter.drawPolyline(&data[firstFrame], lastFrame - firstFrame + 1);

    pen.setColor(Qt::white);

    painter.setPen(pen);
    painter.drawLine(currentFrame, 0, currentFrame, maxval);
}
