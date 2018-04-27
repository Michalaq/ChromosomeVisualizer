#include "lineedit.h"

LineEdit::LineEdit(QWidget *parent) : QLineEdit(parent), multiple(false)
{
    connect(this, &QLineEdit::editingFinished, [this] {
        multiple = false;
    });
}

LineEdit::~LineEdit()
{

}

void LineEdit::setMultipleValues(bool enabled)
{
    bool b = blockSignals(true);

    if (multiple = enabled)
        clear();

    blockSignals(b);
}

#include <QStyle>

void LineEdit::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);

    style()->unpolish(this);
    style()->polish(this);

    update();
}

void LineEdit::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusOutEvent(event);

    style()->unpolish(this);
    style()->polish(this);

    update();
}

#include <QPainter>
#include <QStyleOptionFrameV2>

void LineEdit::paintEvent(QPaintEvent *event)
{
    QLineEdit::paintEvent(event);

    if (multiple && (!hasFocus() || isReadOnly()) && placeholderText().isEmpty())
    {
        QPainter p(this);

        QStyleOptionFrameV2 panel;
        initStyleOption(&panel);
        QRect r = style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);
        r -= textMargins();
        p.setClipRect(r);

        QFontMetrics fm = fontMetrics();
        Qt::Alignment va = QStyle::visualAlignment(layoutDirection(), QFlag(alignment()));
        int vscroll;
        switch (va & Qt::AlignVertical_Mask)
        {
        case Qt::AlignBottom:
            vscroll = r.y() + r.height() - fm.height() - 1;
            break;
        case Qt::AlignTop:
            vscroll = r.y() + 1;
            break;
        default:
            //center
            vscroll = r.y() + (r.height() - fm.height() + 1) / 2;
            break;
        }
        QRect lineRect(r.x() + 2, vscroll, r.width() - 4, fm.height());

        //int minLB = qMax(0, -fm.minLeftBearing());

        //lineRect.adjust(minLB, 0, 0, 0);
        QString elidedText = fm.elidedText("<< multiple values >>", Qt::ElideRight, lineRect.width());
        p.fillRect(lineRect, palette().background());
        p.drawText(lineRect, va, elidedText);
    }
}

void LineEdit::setText(const QString &text, bool spontaneous)
{
    bool b;

    if (!spontaneous)
        b = blockSignals(true);

    multiple = false;
    QLineEdit::setText(text);

    if (!spontaneous)
        blockSignals(b);
}

void LineEdit::insert(const QString &newText, bool spontaneous)
{
    bool b;

    if (!spontaneous)
        b = blockSignals(true);

    multiple = false;
    QLineEdit::insert(newText);

    if (!spontaneous)
        blockSignals(b);
}
