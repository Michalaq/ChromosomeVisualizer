#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QTextEdit>

class TextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit TextEdit(QWidget *parent = 0);
    ~TextEdit();

    void setMultipleValues(bool enabled = true);

protected:
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

signals:

public slots:
    void setText(const QString &text, bool spontaneous = true);

private:
    bool multiple;
};

#endif // TEXTEDIT_H
