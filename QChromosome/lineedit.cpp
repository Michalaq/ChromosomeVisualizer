#include "lineedit.h"

LineEdit::LineEdit(QWidget *parent) : QLineEdit(parent)
{

}

LineEdit::~LineEdit()
{

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
