#include "textedit.h"

TextEdit::TextEdit(QWidget *parent) : QTextEdit(parent)
{

}

TextEdit::~TextEdit()
{

}

#include <QStyle>

void TextEdit::focusInEvent(QFocusEvent *event)
{
    QTextEdit::focusInEvent(event);

    style()->unpolish(this);
    style()->polish(this);

    update();
}

void TextEdit::focusOutEvent(QFocusEvent *event)
{
    QTextEdit::focusOutEvent(event);

    style()->unpolish(this);
    style()->polish(this);

    update();
}
