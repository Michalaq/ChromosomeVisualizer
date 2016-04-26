#include "plot.h"
#include "../QtChromosomeViz_v2/bartekm_code/NullSimulation.h"
#include <QHBoxLayout>
#include "legend.h"

// MathWorks predefined colorOrder
const QList<QColor> Plot::colorOrder = {"#0072bd", "#d95319", "#edb120", "#7e2f8e", "#77ac30", "#4dbeee", "#a2142f"};

Plot::Plot(QWidget *parent) :
    QWidget(parent),
    simulation_(std::make_shared<NullSimulation>()),
    firstFrame(0),
    currentFrame(0),
    lastFrame(0),
    lastBuffered(-1)
{
    new QHBoxLayout(this);
    layout()->setMargin(margin);
    layout()->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
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
    lastBuffered = -1;

    maxval = 0;

    setMinimumHeight(2 * margin + padding_top + padding_bottom);

    for (auto entry : legend)
        entry->deleteLater();

    legend.clear();

    auto color = colorOrder.constBegin();

    for (auto i : simulation_->getFrame(0)->functionValues)
    {
        QString fname = QString::fromStdString(i.first);

        auto entry = new Legend(fname, *color, this);
        connect(entry, SIGNAL(changed()), this, SLOT(update()));
        layout()->addWidget(entry);
        legend[fname] = entry;

        if (++color == colorOrder.constEnd())
            color = colorOrder.constBegin();
    }

    update();
}

void Plot::setMinimum(int m)
{
    firstFrame = m;
    update();
}

void Plot::setMaximum(int m)
{
    if (lastBuffered < m)
    {
        for (int i = lastBuffered + 1; i <= m; i++)
        {
            for (auto entry : simulation_->getFrame(i)->functionValues)
            {
                data[QString::fromStdString(entry.first)] << QPointF(i, entry.second);

                if (maxval < entry.second)
                    maxval = entry.second;
            }
        }

        lastBuffered = m;
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

    if (!simulation_ || firstFrame == lastFrame)
        return;

    QPainter painter(this);

    // draw background
    painter.fillRect(margin, margin, width() - 2 * margin, height() - 2 * margin, "#262626");

    // set coordinate system
    int label = painter.fontMetrics().width(QString::number(qCeil(maxval / 4) * 4));

    painter.setViewport(margin + padding_left + label, height() - padding_bottom - margin, width() - 2 * margin -padding_left - label - padding_right, 2 * margin + padding_top + padding_bottom - height());

    auto transform = painter.combinedTransform();

    // draw axis
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen1(QBrush("#808080"), 1.);
    pen1.setCosmetic(true);

    painter.setPen(pen1);

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

    for (auto i = data.cbegin(); i != data.cend(); i++)
    {
        auto interval = i.value().mid(firstFrame, lastFrame - firstFrame + 1);

        interval.prepend(QPointF(firstFrame, 0));
        interval.append(QPointF(lastFrame, 0));

        QLinearGradient gradient(0, 0, 0, 4 * delta);
        gradient.setColorAt(0, Qt::transparent);
        gradient.setColorAt(1, legend[i.key()]->brush());

        painter.setPen(Qt::NoPen);
        painter.setBrush(gradient);

        painter.drawPolygon(interval);

        QPen pen2(legend[i.key()]->pen(), 2.);
        pen2.setCosmetic(true);

        painter.setPen(pen2);

        painter.drawPolyline(&i.value()[firstFrame], lastFrame - firstFrame + 1);
    }

    QPen pen3(Qt::white, 3.);
    pen3.setCosmetic(true);

    painter.setPen(pen3);
    painter.drawLine(currentFrame, 0, currentFrame, 4 * delta);
}
