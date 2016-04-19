#ifndef SPINBOX_H
#define SPINBOX_H

#include <QSpinBox>

class SpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit SpinBox(QWidget *parent = 0);

protected:
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

private:
    int cachedMinimumWidth;

signals:

public slots:
    void setMaximum(int max);
    void setSoftMaximum(int max);
};

#endif // SPINBOX_H
