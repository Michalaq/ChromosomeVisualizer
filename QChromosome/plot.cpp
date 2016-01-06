#include "plot.h"
#include "../QtChromosomeViz_v2/bartekm_code/NullSimulation.h"

Plot::Plot(QWidget *parent) :
    QWidget(parent),
    simulation_(std::make_shared<NullSimulation>())
{

}

Plot::~Plot()
{

}

void Plot::setSimulation(std::shared_ptr<Simulation> dp)
{
    simulation_ = std::move(dp);

    data = QPainterPath(QPoint(0, simulation_->getFrame(0)->functionValues["bonds"]));
    size = QSize(0, 1);

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

    QRectF rect = data.boundingRect();

    size = QSize(rect.width(), rect.y() + rect.height());

    update();
}

#include <QPainter>

void Plot::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if (!simulation_)
        return;

    QPainter painter(this);

    painter.setWindow(0, height(), width(), -height());
    painter.setRenderHint(QPainter::Antialiasing);

    if (!size.width()) return;

    QPainterPath path(data);
    path.lineTo(data.currentPosition().x(), 0);
    path.lineTo(0, 0);
    path.closeSubpath();

    painter.scale((qreal) width() / size.width(), (qreal) height() / size.height());

    painter.fillPath(path, QColor("#002255"));
}
