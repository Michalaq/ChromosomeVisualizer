#ifndef SPINBOX_H
#define SPINBOX_H

#include <QSpinBox>

class SpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit SpinBox(QWidget *parent = 0);

    QValidator::State validate(QString &input, int &pos) const;
    int valueFromText(const QString &text) const;

protected:
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

signals:

public slots:
    void setValue(int val, bool spontaneous = true);

private:
    QRegularExpression re;
};

#endif // SPINBOX_H
