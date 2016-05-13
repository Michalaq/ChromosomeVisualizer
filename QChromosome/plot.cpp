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

    maxval = -qInf();
    minval = +qInf();

    setMinimumHeight(padding_top + 48 + padding_bottom);

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

                if (minval > entry.second)
                    minval = entry.second;
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
    QSize s;
    s.setHeight(height() - padding_top - padding_bottom);

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

    for (int i = (gap - (softMinimum % gap)) % gap; i <= softMaximum - softMinimum; i += gap)
    {
        auto tick = transform.map(QPoint(i, minval));

        painter.drawLine(tick, tick + QPoint(0, 5));
        painter.drawText(QRect(tick + QPoint(0, padding_left / 2), QSize()), Qt::AlignHCenter | Qt::AlignTop | Qt::TextDontClip, QString::number(softMinimum + i));
    }

    painter.setViewTransformEnabled(true);

    pen1.setStyle(Qt::DashLine);

    painter.setViewTransformEnabled(false);

    for (qreal i = lt; i <= ut; i += delta)
        painter.drawText(QRect(transform.map(QPoint(0, i)) - QPoint(padding_left / 2, 0), QSize()), Qt::AlignRight | Qt::AlignVCenter | Qt::TextDontClip, QString::number(i));

    painter.setViewTransformEnabled(true);

    pen1.setColor("#333333");

    painter.setPen(pen1);

    for (int i = (gap - (softMinimum % gap)) % gap; i <= softMaximum - softMinimum; i += gap)
        painter.drawLine(QPoint(i, minval), QPoint(i, maxval));

    for (qreal i = (lt != minval ? lt : lt + delta); i <= ut; i += delta)
        painter.drawLine(softMinimum, i, softMaximum, i);

    // plot data
    for (auto i = data.cbegin(); i != data.cend(); i++)
    {
        auto interval = i.value().mid(softMinimum, softMaximum - softMinimum + 1);

        interval.prepend(QPointF(softMinimum, 0));
        interval.append(QPointF(softMaximum, 0));

        QLinearGradient gradient(0, minval, 0, maxval);
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
    pen3.setJoinStyle(Qt::MiterJoin);
    pen3.setCosmetic(true);

    painter.setPen(pen3);
    painter.drawLine(value(), minval, value(), maxval);

    painter.setViewTransformEnabled(false);

    auto crs = transform.map(QPoint(value(), maxval));

    painter.setBrush(QBrush(Qt::white));
    painter.drawPolygon(QPolygon({crs, crs + QPoint(4, -5), crs + QPoint(-4, -5)}));
}
