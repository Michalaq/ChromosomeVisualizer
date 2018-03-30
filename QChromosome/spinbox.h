#ifndef SPINBOX_H
#define SPINBOX_H

#include "doublespinbox.h"

class SpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit SpinBox(QWidget *parent = 0);
    ~SpinBox();

    QValidator::State validate(QString &input, int &pos) const;
    int valueFromText(const QString &text) const;
    QString textFromValue(int val) const;

    void setMultipleValues();

    void setMaximum(int max);
    void setMinimum(int min);

protected:
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

signals:

public slots:
    void setValue(int val, bool spontaneous = true);

private:
    RegularExpressionValidator validator;
    LineEdit* edit;
    bool multiple;
};

#endif // SPINBOX_H
