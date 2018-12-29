#ifndef DOUBLESPINBOX_H
#define DOUBLESPINBOX_H

#include <QDoubleSpinBox>
#include "lineedit.h"

class RegularExpressionValidator : public QRegularExpressionValidator
{
public:
    RegularExpressionValidator(const QRegularExpression &rx, QObject *parent = 0);

    State validate(QString &input, int &pos) const;
};

class DoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit DoubleSpinBox(QWidget *parent = 0);
    ~DoubleSpinBox();

    QValidator::State validate(QString &input, int &pos) const;
    double valueFromText(const QString &text) const;
    QString textFromValue(double val) const;

    void setMultipleValues();
    void setSuffix(const QString& suffix);

    void setMaximum(double max, bool spontaneous = true);
    void setMinimum(double min, bool spontaneous = true);

protected:
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

signals:

public slots:
    void setValue(double val, bool spontaneous = true);

private:
    RegularExpressionValidator validator;
    LineEdit* edit;
    bool multiple;
    QString suffix;
};

#endif // DOUBLESPINBOX_H
