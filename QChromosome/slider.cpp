#include "slider.h"
#include <QPainter>

Slider::Slider(QWidget *parent) :
    SoftSlider(parent),
    ip(nullptr),
    s(new QShortcut(QKeySequence(Qt::Key_Delete), this)),
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
    delete s;
}

QSize Slider::minimumSizeHint() const
{
    return QSize(20, 30);
}

void Slider::setInterpolator(Interpolator *_ip)
{
    ip = _ip;
    connect(s, SIGNAL(activated()), ip, SLOT(deleteKeyrame()));
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
        ip->selectKeyframe(sv);
        update();
    }
}

void Slider::mouseMoveEvent(QMouseEvent *event)
{
    auto sv = style()->sliderValueFromPosition(softMinimum, softMaximum, event->pos().x() - 10, width() - 20);

    if (movemarker)
        setValue(sv);
    else
        ip->changeKey(sv, false);
}

void Slider::mouseReleaseEvent(QMouseEvent *event)
{
    auto sv = style()->sliderValueFromPosition(softMinimum, softMaximum, event->pos().x() - 10, width() - 20);

    if (movemarker)
        setValue(sv);
    else
        ip->changeKey(sv);
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

    const auto keys = ip->keys();

    auto it = keys.constBegin();

    while (it != keys.constEnd() && *it < softMinimum) it++;

    p.setPen("#318db9");
    p.setBrush(QBrush("#355668"));

    qreal dx = (width() - 20) / (softMaximum - softMinimum);

    const auto frame = ip->selectedKeyframe();

    for ( ; it != keys.constEnd() && *it <= softMaximum; it++)
        if (*it != frame)
        {
            auto tick = style()->sliderPositionFromValue(softMinimum, softMaximum, *it, width() - 20) + 10;

            p.drawRect(QRect(tick - dx/2, 4, dx, 8));
        }

    if (softMinimum <= frame && frame <= softMaximum)
    {
        p.setPen("#f47750");
        p.setBrush(QBrush("#fcd5c6"));

        auto tick = style()->sliderPositionFromValue(softMinimum, softMaximum, ip->selectedKeyframe(), width() - 20) + 10;

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
        p.fillRect(QRect(tick -  8, -12, p.fontMetrics().width(QString::number(value())) + 18, 16), "#262626");
        p.drawText(QRect(tick + 10, -12, 0, 16), Qt::AlignVCenter | Qt::TextDontClip, QString::number(value()));

        p.drawImage(tick - 15, -15, pin);
    }
}
