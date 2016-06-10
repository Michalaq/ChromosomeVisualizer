#ifndef DOUBLESPINBOX_H
#define DOUBLESPINBOX_H

#include <QDoubleSpinBox>

class DoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit DoubleSpinBox(QWidget *parent = 0);

protected:
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

signals:

public slots:
};

#endif // DOUBLESPINBOX_H