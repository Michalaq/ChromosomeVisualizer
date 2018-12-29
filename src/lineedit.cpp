#include "lineedit.h"

LineEdit::LineEdit(QWidget *parent) : QLineEdit(parent)
{

}

LineEdit::~LineEdit()
{

}

void LineEdit::setMultipleValues(bool enabled)
{
    if (enabled)
    {
        setPlaceholderText("<< multiple values >>");
        clear();
    }
    else
        setPlaceholderText("");
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

void LineEdit::setText(const QString &text, bool spontaneous)
{
    bool b;

    if (!spontaneous)
        b = blockSignals(true);

    QLineEdit::setText(text);
    setPlaceholderText("");

    if (!spontaneous)
        blockSignals(b);
}
