#include "slider.h"

Slider::Slider(QWidget *parent) : SoftSlider(parent), ip(nullptr)
{
    s = new QShortcut(QKeySequence(Qt::Key_Delete), this);

    connect(s, &QShortcut::activated, [this] {
        if (frame != ip->keyframes.end())
        {
            ip->keyframes.erase(frame);
            frame = ip->keyframes.end();
            update();
        }
    });
}

Slider::~Slider()
{
    delete s;
}

QSize Slider::minimumSizeHint() const
{
    return QSize(20, 30);
}

void Slider::setInterpolator(Interpolator *_ip)
{
    ip = _ip;
    frame = ip->keyframes.end();
}

#include <QMouseEvent>
#include <QStyle>

void Slider::mousePressEvent(QMouseEvent *event)
{
    auto sv = style()->sliderValueFromPosition(softMinimum, softMaximum, event->pos().x() - 10, width() - 20);

    if (event->pos().y() < 20)
    {
        movemarker = true;
        setValue(sv);
    }
    else
    {
        movemarker = false;
        frame = ip->keyframes.find(sv);
        update();

        emit keyframeSelected(frame.key());
    }
}

void Slider::mouseMoveEvent(QMouseEvent *event)
{
    auto sv = style()->sliderValueFromPosition(softMinimum, softMaximum, event->pos().x() - 10, width() - 20);

    if (movemarker)
        setValue(sv);
    else
        if (frame != ip->keyframes.end())
        {
            auto v = *frame;
            ip->keyframes.erase(frame);
            frame = ip->keyframes.insertMulti(sv, v);
            update();
        }
}

void Slider::mouseReleaseEvent(QMouseEvent *event)
{
    SoftSlider::mouseReleaseEvent(event);

    if (frame != ip->keyframes.end() && ip->keyframes.count(frame.key()) > 1)
    {
        auto k = frame.key();
        auto v = frame.value();
        ip->keyframes.remove(k);
        frame = ip->keyframes.insert(k, v);
        update();
    }
}

#include <QPainter>
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

    auto it = ip->keyframes.constBegin();

    while (it != ip->keyframes.constEnd() && it.key() < softMinimum) it++;

    p.setPen("#318db9");
    p.setBrush(QBrush("#355668"));

    qreal dx = (width() - 20) / (softMaximum - softMinimum);

    for (; it != ip->keyframes.constEnd() && it.key() <= softMaximum; it++)
        if (it != frame)
        {
            auto tick = style()->sliderPositionFromValue(softMinimum, softMaximum, softMinimum + it.key(), width() - 20) + 10;

            p.drawRect(QRect(tick - dx/2, 4, dx, 8));
        }

    if (frame != ip->keyframes.end() && softMinimum <= frame.key() && frame.key() <= softMaximum)
    {
        p.setPen("#f47750");
        p.setBrush(QBrush("#fcd5c6"));

        auto tick = style()->sliderPositionFromValue(softMinimum, softMaximum, frame.key(), width() - 20) + 10;

        p.drawRect(QRect(tick - dx/2, 4, dx, 8));
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

        p.fillRect(QRect(tick, -12, p.fontMetrics().width(QString::number(value())) + 10, 12), "#262626");
        p.drawText(QRect(tick + 10, -12, 0, 16), Qt::AlignVCenter | Qt::TextDontClip, QString::number(value()));

        p.setPen(Qt::NoPen);
        p.setBrush(QColor("#0072bd"));
        p.setRenderHint(QPainter::Antialiasing);

        p.translate(tick, (1. - qSqrt(2.)) * 5);
        p.drawEllipse(QPoint(), 10, 10);
        p.rotate(45);
        p.drawRect(0, 0, 10, 10);
    }
}
