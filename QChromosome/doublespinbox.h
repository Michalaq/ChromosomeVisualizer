#ifndef DOUBLESPINBOX_H
#define DOUBLESPINBOX_H

#include <QDoubleSpinBox>

class DoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit DoubleSpinBox(QWidget *parent = 0);
    ~DoubleSpinBox();

    QValidator::State validate(QString &input, int &pos) const;
    double valueFromText(const QString &text) const;

protected:
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

signals:

public slots:
    void setValue(double val, bool spontaneous = true);

private:
    QRegularExpression re;
};

#endif // DOUBLESPINBOX_H
