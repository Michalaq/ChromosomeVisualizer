#include "plot.h"
#include <limits>
#include <QHBoxLayout>
#include "../QtChromosomeViz_v2/bartekm_code/NullSimulationLayer.h"
#include "legend.h"
#include <QPainter>
#include <QSvgRenderer>

// MathWorks predefined colorOrder
const QList<QColor> Plot::colorOrder = {"#0072bd", "#d95319", "#edb120", "#7e2f8e", "#77ac30", "#4dbeee", "#a2142f"};

Plot::Plot(QWidget *parent) :
    SoftSlider(parent),
    simulation_(std::make_shared<Simulation>()),
    lastBuffered(-1)
{
    simulation_->addSimulationLayerConcatenation(std::make_shared<SimulationLayerConcatenation>());

    new QHBoxLayout(this);
    layout()->setMargin(0);
    layout()->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
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

    minimax.clear();

    setMinimumHeight(padding_top + 64 + padding_bottom);

    qDeleteAll(legend);

    legend.clear();

//    auto color = colorOrder.constBegin();

//    auto funvals = simulation_->getFrame(1)->functionValues;
//    for (auto i : funvals)
//    {
//        QString fname = QString::fromStdString(i.first);

//        auto entry = new Legend(fname, *color, this);
//        connect(entry, SIGNAL(changed()), this, SLOT(update()));
//        layout()->addWidget(entry);
//        legend[fname] = entry;

//        if (++color == colorOrder.constEnd())
//            color = colorOrder.constBegin();
//    }

    update();
}

void Plot::updateSimulation()
{
    auto oldBuffered = lastBuffered;
    lastBuffered = -1;
    setMaximum(oldBuffered);
}

void Plot::setMaximum(int m)
{
    if (lastBuffered < m)
    {
        int prevI;
        do
        {
            prevI = lastBuffered;
            lastBuffered = simulation_->getNextTime(lastBuffered);

            if (prevI == lastBuffered)
                break;

            auto funvals = simulation_->getFrame(lastBuffered)->functionValues;
            for (auto entry : funvals)
            {
                data[QString::fromStdString(entry.first)] << QPointF(lastBuffered, entry.second);

                minimax.insert(entry.first, lastBuffered, entry.second);
            }
        } while (lastBuffered < m);
    }

    SoftSlider::setMaximum(m);
    update();
}

void Plot::followSlider(QAbstractSlider *s)
{
    slider = s;
    update();
}

#include <QMouseEvent>
#include <QStyle>

void Plot::mousePressEvent(QMouseEvent *event)
{
    if (event->pos().y() > padding_top && event->pos().y() < height() - padding_bottom)
        setValue(style()->sliderValueFromPosition(softMinimum, softMaximum, event->pos().x() - (slider->x() + 10) + x(), slider->width() - 20));
}

void Plot::mouseMoveEvent(QMouseEvent *event)
{
    if (event->pos().y() > padding_top && event->pos().y() < height() - padding_bottom)
        setValue(style()->sliderValueFromPosition(softMinimum, softMaximum, event->pos().x() - (slider->x() + 10) + x(), slider->width() - 20));
}

#include <QtMath>

void Plot::addLegend(const QString &fname)
{
    const auto color = colorOrder[legend.size() % colorOrder.size()];

    auto entry = new Legend(fname, color, this);

    connect(entry, &Legend::changed, [=] {
        minimax.setVisible(fname.toStdString(), entry->pen() != Qt::transparent);
        update();
    });
    layout()->addWidget(entry);
    legend[fname] = entry;
}

void Plot::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if (minimax.empty() || softMinimum == softMaximum)
        return;

    QPainter painter(this);

    // set coordinate system
    QSize s;
    s.setHeight(height() - padding_top - padding_bottom);

    // TODO bug - wykres nie działa, jeśli wartości brzegowe nie są całkowite - problem z setWindow
    double minval = 0;//double minval = qFloor(minimax.minimum(softMinimum, softMaximum));
    double maxval = qCeil(minimax.maximum(softMinimum, softMaximum));

    if (minval == maxval)
        minval--;

    double delta = tickSpan(minval, maxval, s.height(), 24);
    /* TODO do pewnej wysokości nie powinno się rozrzedzać podziałki, tylko ją zwyczajnie skalować */

    double ut = maxval = qCeil(maxval / delta) * delta;//double ut = qFloor(maxval / delta) * delta;
    double lt = qCeil(minval / delta) * delta;

    s.setWidth(slider->width() - 20);

    painter.setViewport((slider->x() + 10) - x(), height() - padding_bottom, s.width(), -s.height());
    painter.setWindow(softMinimum, minval, softMaximum - softMinimum, maxval - minval);

    auto transform = painter.combinedTransform();

    // draw axis
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen1(QBrush("#808080"), 1.);
    pen1.setCosmetic(true);

    painter.setPen(pen1);

    painter.drawLine(softMinimum, minval, softMaximum, minval);

    int gap = tickSpan(painter.fontMetrics().width(QString::number(softMaximum)) + 20, slider->width());

    painter.setViewTransformEnabled(false);

    for (int i = softMinimum + (gap - (softMinimum % gap)) % gap; i <= softMaximum; i += gap)
    {
        auto tick = transform.map(QPoint(i, minval));

        painter.drawLine(tick, tick + QPoint(0, 5));
        painter.drawText(QRect(tick + QPoint(0, 9), QSize()), Qt::AlignHCenter | Qt::AlignTop | Qt::TextDontClip, QString::number(i));
    }

    for (qreal i = lt; i <= ut; i += delta)
        painter.drawText(QRect(transform.map(QPoint(softMinimum, i)) - QPoint(9, 0), QSize()), Qt::AlignRight | Qt::AlignVCenter | Qt::TextDontClip, QString::number(i));

    painter.setViewTransformEnabled(true);

    pen1.setStyle(Qt::DashLine);
    pen1.setColor("#333333");

    painter.setPen(pen1);

    for (qreal i = (lt != minval ? lt : lt + delta); i <= ut; i += delta)
        painter.drawLine(softMinimum, i, softMaximum, i);

    // plot data
    for (auto i = data.cbegin(); i != data.cend(); i++)
    {
        // Ensure we have a legend for this callback type
        if (!legend.contains(i.key()))
            addLegend(i.key());

        const auto & allValues = i.value();
        int lRange, rRange;
        QPointF lPoint = sampleAtX(softMinimum, allValues, nullptr, &lRange);
        QPointF rPoint = sampleAtX(softMaximum, allValues, &rRange, nullptr);

        auto interval = lRange <= rRange ? i.value().mid(lRange, rRange - lRange + 1) : QVector<QPointF>();

        interval.prepend(lPoint);
        interval.prepend(QPointF(lPoint.x(), 0));
        interval.append(rPoint);
        interval.append(QPointF(rPoint.x(), 0));

        QLinearGradient gradient(0, minval, 0, maxval);
        gradient.setColorAt(0, Qt::transparent);
        gradient.setColorAt(1, legend[i.key()]->brush());

        painter.setPen(Qt::NoPen);
        painter.setBrush(gradient);

        painter.drawPolygon(interval);

        QPen pen2(legend[i.key()]->pen(), 2.);
        pen2.setCosmetic(true);

        painter.setPen(pen2);

        painter.drawPolyline(&interval[1], interval.size() - 2);
    }

    // draw handle
    if (softMinimum <= value() && value() <= softMaximum)
    {
        painter.setViewTransformEnabled(false);

        auto crs = transform.map(QPoint(value(), 0)).x();

        QPen pen3(QColor("#0072bd"), 2.);
        pen3.setJoinStyle(Qt::MiterJoin);

        painter.setPen(pen3);
        painter.drawLine(crs, 0, crs, height() - padding_bottom);
    }
}


QPointF Plot::sampleAtX(qreal x, const QVector<QPointF> &plot, int *lowerIndex, int *upperIndex)
{
    QPointF ret;
    int lower, upper;

    auto compare = [](const QPointF & pt, qreal x) {
        return pt.x() < x;
    };

    auto it = std::lower_bound(plot.begin(), plot.end(), x, compare);

    if (it == plot.end())
    {
        lower = upper = plot.size() - 1;
        ret = QPointF(x, plot.last().y());
    }
    else if (it == plot.begin())
    {
        lower = -1;
        upper = 0;
        ret = QPointF(x, plot.first().y());
    }
    else
    {
        upper = std::distance(plot.begin(), it);
        lower = upper - 1;

        QPointF p1 = plot[lower], p2 = plot[upper];
        qreal t = (x - p1.x()) / (p2.x() - p1.x());
        ret = QPointF(x, p1.y() + t * (p2.y() - p1.y()));
    }

    if (lowerIndex)
        *lowerIndex = lower;
    if (upperIndex)
        *upperIndex = upper;

    return ret;
}
