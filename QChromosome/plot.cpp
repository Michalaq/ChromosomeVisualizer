#include "plot.h"
#include <QHBoxLayout>
#include "session.h"

// MathWorks predefined colorOrder
const QList<QColor> Plot::colorOrder = {"#0072bd", "#d95319", "#edb120", "#7e2f8e", "#77ac30", "#4dbeee", "#a2142f"};

Plot::Plot(Session* s, QWidget *parent) :
    SoftSlider(parent),
    session(s)
{
    setMaximum(0);
    setMinimumHeight(padding_top + 64 + padding_bottom);

    new QHBoxLayout(this);
    layout()->setMargin(9);
    layout()->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    connect(this, &QSlider::valueChanged, s, &Session::setDocumentTime);
}

Plot::~Plot()
{

}

#include <QMouseEvent>
#include <QStyle>

void Plot::mousePressEvent(QMouseEvent *event)
{
    if (event->pos().y() > padding_top && event->pos().y() < height() - padding_bottom)
    {
        auto sv = style()->sliderValueFromPosition(softMinimum, softMaximum, event->pos().x() - 50 - 10, width() - 50 - 20);
        setValue(sv);
        emit sliderPressed();
    }
}

void Plot::mouseMoveEvent(QMouseEvent *event)
{
    if (event->pos().y() > padding_top && event->pos().y() < height() - padding_bottom)
    {
        auto sv = style()->sliderValueFromPosition(softMinimum, softMaximum, event->pos().x() - 50 - 10, width() - 50 - 20);
        setValue(sv);
        emit sliderMoved(sv);
    }
}

void Plot::mouseReleaseEvent(QMouseEvent *event)
{
    SoftSlider::mouseReleaseEvent(event);
    emit sliderReleased();
}

void Plot::addLegend(QtCharts::QAbstractSeries *series)
{
    const auto color = colorOrder[legend.size() % colorOrder.size()];

    auto entry = new Legend(series->name(), color, this);

    connect(entry, SIGNAL(changed()), this, SLOT(update()));
    layout()->addWidget(entry);
    legend[series] = entry;
}

#include <QPainter>
#include <QtMath>
#include <QLineSeries>

void Plot::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if (session->chart->series().isEmpty() || softMinimum == softMaximum)
        return;

    QPainter painter(this);

    // set coordinate system
    QSize s;
    s.setHeight(height() - padding_top - padding_bottom);

    // TODO bug - wykres nie działa, jeśli wartości brzegowe nie są całkowite - problem z setWindow
    double minval = 0;//double minval = qFloor(minimax.minimum(softMinimum, softMaximum));
    double maxval = 0;//qCeil(minimax.maximum(softMinimum, softMaximum));
    for (auto i : session->chart->series())
        for (int j = softMinimum; j <= softMaximum && j < reinterpret_cast<QtCharts::QLineSeries*>(i)->count(); j++)
            maxval = qMax(maxval, reinterpret_cast<QtCharts::QLineSeries*>(i)->at(j).y());

    if (minval == maxval)
        minval--;

    double delta = tickSpan(minval, maxval, s.height(), 24);
    /* TODO do pewnej wysokości nie powinno się rozrzedzać podziałki, tylko ją zwyczajnie skalować */

    double ut = maxval = qCeil(maxval / delta) * delta;//double ut = qFloor(maxval / delta) * delta;
    double lt = qCeil(minval / delta) * delta;

    s.setWidth(width() - 50 - 20);

    painter.setViewport(50 + 10, height() - padding_bottom, s.width(), -s.height());
    painter.setWindow(softMinimum, minval, softMaximum - softMinimum, maxval - minval);

    auto transform = painter.combinedTransform();

    // draw axis
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen1(QBrush("#808080"), 1.);
    pen1.setCosmetic(true);

    painter.setPen(pen1);

    painter.drawLine(softMinimum, minval, softMaximum, minval);

    int gap = tickSpan(painter.fontMetrics().width(QString::number(softMaximum)) + 20, width() - 50);

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
    for (auto i : session->chart->series())
    {
        // Ensure we have a legend for this callback type
        if (!legend.contains(i))
            addLegend(i);

        auto interval = reinterpret_cast<QtCharts::QLineSeries*>(i)->pointsVector().mid(softMinimum, softMaximum - softMinimum + 1);
        interval.prepend({interval.first().x(), 0});
        interval.append({interval.last().x(), 0});

        QLinearGradient gradient(0, minval, 0, maxval);
        gradient.setColorAt(0, Qt::transparent);
        gradient.setColorAt(1, legend[i]->brush());

        painter.setPen(Qt::NoPen);
        painter.setBrush(gradient);

        painter.drawPolygon(interval);

        QPen pen2(legend[i]->pen(), 2.);
        pen2.setCosmetic(true);

        painter.setPen(pen2);

        painter.drawPolyline(&interval[1], interval.size() - 2);
    }

    if (softMinimum <= value() && value() <= softMaximum)
    {
        painter.setViewTransformEnabled(false);

        auto crs = transform.map(QPoint(value(), 0)).x();

        QPen pen3(Qt::white, 2.);
        pen3.setJoinStyle(Qt::MiterJoin);

        painter.setPen(pen3);
        painter.setBrush(QBrush(Qt::white));
        painter.drawLine(crs, padding_top - 5, crs, height() - padding_bottom);
        painter.drawConvexPolygon(QPolygon({{crs, padding_top - 2}, {crs - 3, padding_top - 5}, {crs + 3, padding_top - 5}}));
    }
}
