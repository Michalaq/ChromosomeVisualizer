#include "plot.h"
#include "../QtChromosomeViz_v2/bartekm_code/NullSimulation.h"

Plot::Plot(QWidget *parent) :
    QWidget(parent),
    simulation_(std::make_shared<NullSimulation>()),
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

    data = QPainterPath(QPoint(0, simulation_->getFrame(0)->functionValues["bonds"]));

    databr = data.boundingRect();

    lastFrame = 0;

    update();
}

void Plot::setMaximum(int m)
{
    for (int i = lastFrame + 1; i <= m; i++)
    {
        qreal y = simulation_->getFrame(i)->functionValues["bonds"];
        data.lineTo(i, y);
    }

    lastFrame = m;

    databr = data.boundingRect();

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

    if (!simulation_ || !databr.width() || !databr.height())
        return;

    QPainter painter(this);

    painter.setWindow(0, height(), width(), -height());
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path(data);
    path.lineTo(data.currentPosition().x(), databr.y());
    path.lineTo(0, databr.y());
    path.closeSubpath();

    painter.translate(12, 12);
    painter.scale((qreal) (width() - 24) / databr.width(), (qreal) (height() - 24) / databr.height());
    painter.translate(-databr.topLeft());

    painter.fillPath(path, QColor("#002255"));

    QPen pen(QColor("#003380"), 2., Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    pen.setCosmetic(true);

    painter.setPen(pen);
    painter.drawPath(data);

    pen.setColor(Qt::white);

    painter.setPen(pen);
    painter.drawLine(currentFrame, databr.y(), currentFrame, databr.y() + databr.height());
    //QPointF(data.elementAt(currentFrame)).y()
}
