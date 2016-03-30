#include "plot.h"
#include "../QtChromosomeViz_v2/bartekm_code/NullSimulation.h"

Plot::Plot(QWidget *parent) :
    QWidget(parent),
    simulation_(std::make_shared<NullSimulation>()),
    firstFrame(0),
    currentFrame(0),
    lastFrame(0)
{
    setMinimumHeight(15 + 40 + 50 + 15);
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

    // draw background
    painter.fillRect(15, 15, width() - 30, height() - 30, "#262626");

    // set coordinate system
    painter.setViewport(15 + 30, height() - 50 - 15, width() - 15 - 30 - 15 - 15, 15 + 40 + 50 + 15 - height());

    // draw axis
    QPen pen1(QBrush("#808080"), 1.0, Qt::SolidLine);
    pen1.setCosmetic(true);

    painter.setPen(pen1);

    painter.drawLine(0, 0, width(), 0);

    pen1.setStyle(Qt::DashLine);

    painter.setPen(pen1);

    for (int i = 1; i < 5; i++)
        painter.drawLine(0, height() * i / 4, width(), height() * i / 4);

    auto transform = painter.combinedTransform();

    painter.setViewTransformEnabled(false);

    int delta = int((maxval + 100) / 100) * 25;

    for (int i = 0; i < 5; i++)
        painter.drawText(QRect(transform.map(QPoint(0, height() * i / 4)) - QPoint(30, 15), QSize(25, 30)), Qt::AlignRight | Qt::AlignVCenter, QString::number(delta * i));

    painter.setViewTransformEnabled(true);

    // plot data
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setWindow(firstFrame, 0, lastFrame - firstFrame, 4 * delta);

    auto interval = data.mid(firstFrame, lastFrame - firstFrame + 1);

    interval.prepend(QPointF(firstFrame, 0));
    interval.append(QPointF(lastFrame, 0));

    painter.save();

    QLinearGradient gradient(0, 0, 0, 4 * delta);
    gradient.setColorAt(0, Qt::transparent);
    gradient.setColorAt(1, "#803771c8");

    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient);

    painter.drawPolygon(interval);

    painter.restore();

    QPen pen2(QBrush("#2a7fff"), 2., Qt::SolidLine);
    pen2.setCosmetic(true);

    painter.setPen(pen2);

    painter.drawPolyline(&data[firstFrame], lastFrame - firstFrame + 1);

    QPen pen3(Qt::white, 3.);
    pen3.setCosmetic(true);

    painter.setPen(pen3);
    painter.drawLine(currentFrame, 0, currentFrame, 4 * delta);
}
