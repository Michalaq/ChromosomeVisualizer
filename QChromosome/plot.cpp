#include "plot.h"
#include "../QtChromosomeViz_v2/bartekm_code/NullSimulation.h"
#include <QHBoxLayout>
#include "legend.h"

// MathWorks predefined colorOrder
const QList<QColor> Plot::colorOrder = {"#0072bd", "#d95319", "#edb120", "#7e2f8e", "#77ac30", "#4dbeee", "#a2142f"};

Plot::Plot(QWidget *parent) :
    SoftSlider(parent),
    simulation_(std::make_shared<NullSimulation>()),
    lastBuffered(-1)
{
    new QHBoxLayout(this);
    layout()->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
}

Plot::~Plot()
{

}

void Plot::setSimulation(std::shared_ptr<Simulation> dp)
{
    simulation_ = std::move(dp);

    data.clear();

    setRange(0, 0);
    lastBuffered = -1;

    maxval = 0;

    setMinimumHeight(padding_top + padding_bottom);

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

    SoftSlider::setMaximum(m);
    update();
}

#include <QMouseEvent>
#include <QStyle>

void Plot::mousePressEvent(QMouseEvent *event)
{
    setValue(style()->sliderValueFromPosition(softMinimum, softMaximum, event->pos().x() - padding_left - label, width() - padding_left - label - padding_right));
}

void Plot::mouseMoveEvent(QMouseEvent *event)
{
    setValue(style()->sliderValueFromPosition(softMinimum, softMaximum, event->pos().x() - padding_left - label, width() - padding_left - label - padding_right));
}

#include <QPainter>
#include <QtMath>

void Plot::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if (!simulation_ || softMinimum == softMaximum)
        return;

    QPainter painter(this);

    // set coordinate system
    label = painter.fontMetrics().width(QString::number(qCeil(maxval / 4) * 4));

    painter.setViewport(padding_left + label, height() - padding_bottom, width() - padding_left - label - padding_right, padding_top + padding_bottom - height());

    auto transform = painter.combinedTransform();

    // draw axis
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen1(QBrush("#808080"), 1.);
    pen1.setCosmetic(true);

    painter.setPen(pen1);

    painter.drawLine(0, 0, width(), 0);

    qreal jmp = qreal(painter.fontMetrics().width(QString::number(softMaximum)) + 20) * (softMaximum - softMinimum) / width();

    int b = 1;

    while (10 * b <= jmp)
        b *= 10;

    int q = qCeil(jmp / b);

    int gap;

    if (q > 5)
        gap = 10 * b;
    else
    {
        if (q > 2)
            gap = 5 * b;
        else
        {
            if (q > 1)
                gap = 2 * b;
            else
                gap = 1 * b;
        }
    }

    painter.setViewTransformEnabled(false);

    for (int i = (gap - (softMinimum % gap)) % gap; i <= softMaximum - softMinimum; i += gap)
    {
        auto tick = transform.map(QPoint(style()->sliderPositionFromValue(softMinimum, softMaximum, softMinimum + i, width()), 0));

        painter.drawLine(tick, tick + QPoint(0, 5));
        painter.drawText(QRect(tick + QPoint(0, padding_left / 2), QSize()), Qt::AlignHCenter | Qt::AlignTop | Qt::TextDontClip, QString::number(softMinimum + i));
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
    painter.setWindow(softMinimum, 0, softMaximum - softMinimum, 4 * delta);

    for (auto i = data.cbegin(); i != data.cend(); i++)
    {
        auto interval = i.value().mid(softMinimum, softMaximum - softMinimum + 1);

        interval.prepend(QPointF(softMinimum, 0));
        interval.append(QPointF(softMaximum, 0));

        QLinearGradient gradient(0, 0, 0, 4 * delta);
        gradient.setColorAt(0, Qt::transparent);
        gradient.setColorAt(1, legend[i.key()]->brush());

        painter.setPen(Qt::NoPen);
        painter.setBrush(gradient);

        painter.drawPolygon(interval);

        QPen pen2(legend[i.key()]->pen(), 2.);
        pen2.setCosmetic(true);

        painter.setPen(pen2);

        painter.drawPolyline(&i.value()[softMinimum], softMaximum - softMinimum + 1);
    }

    QPen pen3(Qt::white, 2.);
    pen3.setCosmetic(true);

    painter.setPen(pen3);
    painter.drawLine(value(), 0, value(), 4 * delta);
}
