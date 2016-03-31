#include "plot.h"
#include "../QtChromosomeViz_v2/bartekm_code/NullSimulation.h"
#include <QHBoxLayout>
#include "legend.h"

Plot::Plot(QWidget *parent) :
    QWidget(parent),
    simulation_(std::make_shared<NullSimulation>()),
    firstFrame(0),
    currentFrame(0),
    lastFrame(0)
{
    new QHBoxLayout(this);
    layout()->setMargin(margin);
    layout()->setAlignment(Qt::AlignBottom);
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

    setMinimumHeight(2 * margin + padding_top + padding_bottom);

    while (QLayoutItem* child = layout()->takeAt(0))
    {
        delete child->widget();
        delete child;
    }

    layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
    layout()->addWidget(new Legend("bonds", this));
    layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));

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

    QPen pen1(QBrush("#808080"), 1.0, Qt::SolidLine);
    pen1.setCosmetic(true);

    painter.setPen(pen1);

    int label = painter.fontMetrics().width(QString::number(qCeil(maxval / 4) * 4));

    // set coordinate system
    painter.setViewport(margin + padding_left + label, height() - padding_bottom - margin, width() - 2 * margin -padding_left - label - padding_right, 2 * margin + padding_top + padding_bottom - height());

    auto transform = painter.combinedTransform();

    // draw axis
    painter.drawLine(0, 0, width(), 0);

    int gap = qCeil(qreal(painter.fontMetrics().width(QString::number(lastFrame)) + 15) * (lastFrame - firstFrame) / width());

    painter.setViewTransformEnabled(false);

    for (int i = 0; i <= lastFrame - firstFrame; i += gap)
    {
        auto tick = transform.map(QPoint(width() * i / (lastFrame - firstFrame), 0));

        painter.drawLine(tick, tick + QPoint(0, 5));
        painter.drawText(QRect(tick + QPoint(0, padding_left / 2), QSize()), Qt::AlignHCenter | Qt::AlignTop | Qt::TextDontClip, QString::number(firstFrame + i));
    }

    painter.setViewTransformEnabled(true);

    pen1.setStyle(Qt::DashLine);

    painter.setPen(pen1);

    for (int i = 1; i < 5; i++)
        painter.drawLine(0, height() * i / 4, width(), height() * i / 4);

    painter.setViewTransformEnabled(false);

    int delta = qCeil(maxval / 4);

    for (int i = 0; i < 5; i++)
        painter.drawText(QRect(transform.map(QPoint(0, height() * i / 4)) - QPoint(padding_left / 2, 0), QSize()), Qt::AlignRight | Qt::AlignVCenter | Qt::TextDontClip, QString::number(delta * i));

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

    QPen pen2(QBrush("#2a7fff"), 2., Qt::SolidLine);
    pen2.setCosmetic(true);

    painter.setPen(pen2);

    painter.drawPolyline(&data[firstFrame], lastFrame - firstFrame + 1);

    QPen pen3(Qt::white, 3.);
    pen3.setCosmetic(true);

    painter.setPen(pen3);
    painter.drawLine(currentFrame, 0, currentFrame, 4 * delta);
}
