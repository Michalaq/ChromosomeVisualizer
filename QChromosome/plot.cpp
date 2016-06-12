#include "plot.h"
#include <limits>
#include <QHBoxLayout>
#include "../QtChromosomeViz_v2/bartekm_code/NullSimulationLayer.h"
#include "legend.h"

// MathWorks predefined colorOrder
const QList<QColor> Plot::colorOrder = {"#0072bd", "#d95319", "#edb120", "#7e2f8e", "#77ac30", "#4dbeee", "#a2142f"};

Plot::Plot(QWidget *parent) :
    SoftSlider(parent),
    simulation_(std::make_shared<Simulation>()),
    lastBuffered(-1)
{
    simulation_->addSimulationLayerConcatenation(std::make_shared<SimulationLayerConcatenation>());

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

    minimax.clear();

    setMinimumHeight(padding_top + 48 + padding_bottom);

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

#include <QMouseEvent>
#include <QStyle>

void Plot::mousePressEvent(QMouseEvent *event)
{
    if (event->pos().y() > padding_top && event->pos().y() < height() - padding_bottom)
        setValue(style()->sliderValueFromPosition(softMinimum, softMaximum, event->pos().x() - padding_left - label, width() - padding_left - label - padding_right));
}

void Plot::mouseMoveEvent(QMouseEvent *event)
{
    if (event->pos().y() > padding_top && event->pos().y() < height() - padding_bottom)
        setValue(style()->sliderValueFromPosition(softMinimum, softMaximum, event->pos().x() - padding_left - label, width() - padding_left - label - padding_right));
}

#include <QPainter>
#include <QtMath>

void Plot::addLegend(const QString &fname)
{
    const auto color = colorOrder[legend.size() % colorOrder.size()];

    auto entry = new Legend(fname, color, this);
    connect(entry, &Legend::changed, [=] {
        minimax.setVisible(fname.toStdString(), entry->pen().alpha() != 0);
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

    double minval = minimax.minimum(softMinimum, softMaximum);
    double maxval = minimax.maximum(softMinimum, softMaximum);

    if (minval == maxval)
        minval--;

    double delta = tickSpan(minval, maxval, s.height(), 24);
    /* TODO do pewnej wysokości nie powinno się rozrzedzać podziałki, tylko ją zwyczajnie skalować */

    double ut = qFloor(maxval / delta) * delta;
    double lt = qCeil(minval / delta) * delta;

    label = painter.fontMetrics().width(QString::number(delta != qInf() ? ut : 0));

    s.setWidth(width() - padding_left - label - padding_right);

    painter.setViewport(padding_left + label, height() - padding_bottom, s.width(), -s.height());
    painter.setWindow(softMinimum, minval, softMaximum - softMinimum, maxval - minval);

    auto transform = painter.combinedTransform();

    // draw axis
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen1(QBrush("#808080"), 1.);
    pen1.setCosmetic(true);

    painter.setPen(pen1);

    painter.drawLine(softMinimum, minval, softMaximum, minval);

    int gap = tickSpan(painter.fontMetrics().width(QString::number(softMaximum)) + 20);

    painter.setViewTransformEnabled(false);

    for (int i = softMinimum + (gap - (softMinimum % gap)) % gap; i <= softMaximum; i += gap)
    {
        auto tick = transform.map(QPoint(i, minval));

        painter.drawLine(tick, tick + QPoint(0, 5));
        painter.drawText(QRect(tick + QPoint(0, padding_left / 2), QSize()), Qt::AlignHCenter | Qt::AlignTop | Qt::TextDontClip, QString::number(i));
    }

    for (qreal i = lt; i <= ut; i += delta)
        painter.drawText(QRect(transform.map(QPoint(softMinimum, i)) - QPoint(padding_left / 2, 0), QSize()), Qt::AlignRight | Qt::AlignVCenter | Qt::TextDontClip, QString::number(i));

    painter.setViewTransformEnabled(true);

    pen1.setStyle(Qt::DashLine);
    pen1.setColor("#333333");

    painter.setPen(pen1);

    for (int i = softMinimum + (gap - (softMinimum % gap)) % gap; i <= softMaximum; i += gap)
        painter.drawLine(QPoint(i, minval), QPoint(i, maxval));

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

        auto interval = i.value().mid(lRange, rRange - lRange + 1);

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

    if (softMinimum <= value() && value() <= softMaximum)
    {
        QPen pen3(Qt::white, 2.);
        pen3.setJoinStyle(Qt::MiterJoin);
        pen3.setCosmetic(true);

        painter.setPen(pen3);
        painter.drawLine(value(), minval, value(), maxval);

        painter.setViewTransformEnabled(false);

        auto crs = transform.map(QPoint(value(), maxval));

        painter.setBrush(QBrush(Qt::white));
        painter.drawPolygon(QPolygon({crs, crs + QPoint(4, -5), crs + QPoint(-4, -5)}));
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
        lower = upper = 0;
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
