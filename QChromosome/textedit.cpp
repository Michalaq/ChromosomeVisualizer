#include "textedit.h"

TextEdit::TextEdit(QWidget *parent) : QTextEdit(parent), multiple(false)
{
    connect(this, &TextEdit::textChanged, [this] {
        multiple = false;
    });
}

TextEdit::~TextEdit()
{

}

void TextEdit::setMultipleValues(bool enabled)
{
    if (multiple = enabled)
    {
        bool b = blockSignals(true);
        QTextEdit::setText("<< multiple values >>");
        blockSignals(b);
    }
}

void TextEdit::setText(const QString &text, bool spontaneous)
{
    bool b;

    if (!spontaneous)
        b = blockSignals(true);

    multiple = false;
    QTextEdit::setText(text);

    if (!spontaneous)
        blockSignals(b);
}

#include <QStyle>

void TextEdit::focusInEvent(QFocusEvent *event)
{
    QTextEdit::focusInEvent(event);

    style()->unpolish(this);
    style()->polish(this);

    if (multiple)
    {
        bool b = blockSignals(true);
        clear();
        blockSignals(b);
    }

    update();
}

void TextEdit::focusOutEvent(QFocusEvent *event)
{
    QTextEdit::focusOutEvent(event);

    style()->unpolish(this);
    style()->polish(this);

    if (multiple)
    {
        bool b = blockSignals(true);
        QTextEdit::setText("<< multiple values >>");
        blockSignals(b);
    }

    update();
}
