#include "slider.h"
#include <QPainter>
#include <QSvgRenderer>

Slider::Slider(QWidget *parent) :
    SoftSlider(parent),
    interpolator(nullptr),
    pin(QImage(QSize(30, 30), QImage::Format_ARGB32_Premultiplied))
{
    pin.fill("#0066ff");

    QImage image(30, 30, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    QSvgRenderer(QString(":/location")).render(&painter);

    pin.setAlphaChannel(image.alphaChannel());
}

Slider::~Slider()
{

}

QSize Slider::minimumSizeHint() const
{
    return QSize(20, 30);
}

void Slider::setSplineInterpolator(SplineInterpolator *si)
{
    if (interpolator)
        interpolator->disconnect(this);

    interpolator = si;

    connect(interpolator, SIGNAL(interpolationChanged()), this, SLOT(update()));
    connect(interpolator, SIGNAL(selectionChanged()), this, SLOT(update()));

    update();
}

#include <QMouseEvent>
#include <QStyle>

void Slider::mousePressEvent(QMouseEvent *event)
{
    movemarker = event->pos().y() < 20;

    auto sv = style()->sliderValueFromPosition(softMinimum, softMaximum, event->pos().x() - 10, width() - 20);

    if (movemarker)
        setValue(sv);
    else
        if (interpolator)
        {
            interpolator->select(sv, QItemSelectionModel::ClearAndSelect);//QApplication::keyboardModifiers();
            lastValue = sv;
        }
}

void Slider::mouseMoveEvent(QMouseEvent *event)
{
    auto sv = style()->sliderValueFromPosition(softMinimum, softMaximum, event->pos().x() - 10, width() - 20);

    if (movemarker)
        setValue(sv);
    else
        if (interpolator)
        {
            interpolator->adjustKeys(sv - lastValue);
            lastValue = sv;
        }
}

void Slider::mouseReleaseEvent(QMouseEvent *event)
{
    auto sv = style()->sliderValueFromPosition(softMinimum, softMaximum, event->pos().x() - 10, width() - 20);

    if (movemarker)
        setValue(sv);
    else
        if (interpolator)
            interpolator->adjustKeys(sv - lastValue);
}

#include <QtMath>

void Slider::paintEvent(QPaintEvent *event)
{
    QAbstractSlider::paintEvent(event);

    QPainter p(this);
    p.translate(0, height() / 2);

    p.fillRect(QRect(0, -12, width(), 24), "#262626");

    if (softMinimum == softMaximum)
        return;

    p.setPen("#4d4d4d");

    p.drawLine(QPoint(0, 4), QPoint(width(), 4));

    if (interpolator)
    {
        p.setPen("#377c9f");

        qreal dx = (width() - 20) / (softMaximum - softMinimum);

        auto it = interpolator->keyBegin();

        while (it != interpolator->keyEnd() && *it < softMinimum) it++;

        for ( ; it != interpolator->keyEnd() && *it <= softMaximum; it++)
        {
            p.setBrush(QBrush(interpolator->isSelected(*it) ? "#377c9f" : "#33576b"));
            auto tick = style()->sliderPositionFromValue(softMinimum, softMaximum, *it, width() - 20) + 10;
            p.drawRect(QRect(tick - dx/2, 4, dx, 8));
        }
    }

    p.setPen(Qt::white);

    int gap = tickSpan(p.fontMetrics().width(QString::number(softMaximum)) + 20);

    for (int i = (gap - (softMinimum % gap)) % gap; i <= softMaximum - softMinimum; i += gap)
    {
        auto tick = style()->sliderPositionFromValue(softMinimum, softMaximum, softMinimum + i, width() - 20) + 10;

        p.drawLine(QPoint(tick, 4), QPoint(tick, 12));
        p.drawText(QRect(tick, -12, 0, 16), Qt::AlignCenter | Qt::TextDontClip, QString::number(softMinimum + i));
    }

    int sgap = gap >= 10 ? gap / 10 : 1;

    for (int i = (sgap - (softMinimum % sgap)) % sgap; i <= softMaximum - softMinimum; i += sgap)
    {
        if (((softMinimum + i) % gap) == 0)
            continue;

        auto tick = style()->sliderPositionFromValue(softMinimum, softMaximum, softMinimum + i, width() - 20) + 10;

        p.drawLine(QPoint(tick, 8), QPoint(tick, 12));
    }

    if (softMinimum <= value() && value() <= softMaximum)
    {
        auto tick = style()->sliderPositionFromValue(softMinimum, softMaximum, value(), width() - 20) + 10;

        p.setPen("#0072bd");
        p.fillRect(QRect(tick -  7, -12, p.fontMetrics().width(QString::number(value())) + 17, 16), "#262626");
        p.drawText(QRect(tick + 10, -12, 0, 16), Qt::AlignVCenter | Qt::TextDontClip, QString::number(value()));

        p.drawImage(tick - 15, -15, pin);
    }
}

void Slider::keyPressEvent(QKeyEvent *event)
{
    QSlider::keyPressEvent(event);

    if (interpolator && (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace))
    {
        interpolator->remove();
        update();
    }
}
