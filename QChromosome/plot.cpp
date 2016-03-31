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
#include <QtMath>

void Plot::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if (!simulation_ || data.isEmpty())
        return;

    QPainter painter(this);

    // draw background
    painter.fillRect(margin, margin, width() - 2 * margin, height() - 2 * margin, "#262626");

    // draw legend
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen1(QBrush("#2a7fff"), 2., Qt::SolidLine);
    pen1.setCosmetic(true);

    painter.setPen(pen1);

    painter.drawEllipse(width() / 2 - 30, height() - 35, 10, 10);

    QPen pen2(QBrush("#808080"), 1.0, Qt::SolidLine);
    pen2.setCosmetic(true);

    painter.setPen(pen2);

    painter.drawText(QRect(width() / 2 - 15, height() - 35, 45, 10), Qt::AlignLeft | Qt::AlignVCenter, "bonds");

    int _padding_left = padding_left + painter.fontMetrics().width(QString::number(qCeil(maxval / 4) * 4));

    // set coordinate system
    painter.setViewport(margin + _padding_left, height() - padding_bottom - margin, width() - 2 * margin - _padding_left - padding_right, 2 * margin + padding_top + padding_bottom - height());

    auto transform = painter.combinedTransform();

    // draw axis
    painter.drawLine(0, 0, width(), 0);

    int gap = qCeil(25. * (lastFrame - firstFrame) / width());

    painter.setViewTransformEnabled(false);

    for (int i = 0; i <= lastFrame - firstFrame; i += gap)
    {
        auto tick = transform.map(QPoint(width() * i / (lastFrame - firstFrame), 0));

        painter.drawLine(tick, tick + QPoint(0, 5));
        painter.drawText(QRect(tick + QPoint(-10, 5), QSize(20, 20)), Qt::AlignCenter, QString::number(firstFrame + i));
    }

    painter.setViewTransformEnabled(true);

    pen2.setStyle(Qt::DashLine);

    painter.setPen(pen2);

    for (int i = 1; i < 5; i++)
        painter.drawLine(0, height() * i / 4, width(), height() * i / 4);

    painter.setViewTransformEnabled(false);

    int delta = qCeil(maxval / 4);

    for (int i = 0; i < 5; i++)
        painter.drawText(QRect(transform.map(QPoint(0, height() * i / 4)) - QPoint(_padding_left, 10), QSize(_padding_left - padding_left / 2, 20)), Qt::AlignRight | Qt::AlignVCenter, QString::number(delta * i));

    painter.setViewTransformEnabled(true);

    // plot data
    painter.setWindow(firstFrame, 0, lastFrame - firstFrame, 4 * delta);

    auto interval = data.mid(firstFrame, lastFrame - firstFrame + 1);

    interval.prepend(QPointF(firstFrame, 0));
    interval.append(QPointF(lastFrame, 0));

    QLinearGradient gradient(0, 0, 0, 4 * delta);
    gradient.setColorAt(0, Qt::transparent);
    gradient.setColorAt(1, "#803771c8");

    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient);

    painter.drawPolygon(interval);

    painter.setPen(pen1);

    painter.drawPolyline(&data[firstFrame], lastFrame - firstFrame + 1);

    QPen pen3(Qt::white, 3.);
    pen3.setCosmetic(true);

    painter.setPen(pen3);
    painter.drawLine(currentFrame, 0, currentFrame, 4 * delta);
}
