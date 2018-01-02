#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QLineEdit>

class LineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit LineEdit(QWidget *parent = 0);
    ~LineEdit();

    void setMultipleValues();

protected:
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void paintEvent(QPaintEvent *event);

signals:

public slots:
    void setText(const QString& text, bool spontaneous = true);
    void insert(const QString &newText, bool spontaneous = true);

private:
    bool multiple;
};

#endif // LINEEDIT_H
