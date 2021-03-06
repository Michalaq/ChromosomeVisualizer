#include "doublespinbox.h"

RegularExpressionValidator::RegularExpressionValidator(const QRegularExpression &rx, QObject *parent) : QRegularExpressionValidator(rx, parent)
{

}

QValidator::State RegularExpressionValidator::validate(QString &input, int &) const
{
    return regularExpression().match(input).hasMatch() ? QValidator::Acceptable : QValidator::Intermediate;
}

DoubleSpinBox::DoubleSpinBox(QWidget *parent) :
    QDoubleSpinBox(parent),
    validator(QRegularExpression(QString("[+-]?\\d+(%1\\d+)?").arg(QRegularExpression::escape(locale().decimalPoint()))), this),
    edit(new LineEdit(this)),
    multiple(false)
{
    setKeyboardTracking(false);

    edit->setValidator(&validator);
    setLineEdit(edit);

    connect(this, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val) {
        setValue(val, false);
    });
}

DoubleSpinBox::~DoubleSpinBox()
{

}

void DoubleSpinBox::setValue(double val, bool spontaneous)
{
    bool b;

    if (!spontaneous)
        b = blockSignals(true);

    QDoubleSpinBox::setSuffix(suffix);
    edit->setMultipleValues(multiple = false);
    QDoubleSpinBox::setValue(val);

    if (!spontaneous)
        blockSignals(b);
}

QValidator::State DoubleSpinBox::validate(QString &input, int &pos) const
{
    return validator.validate(input, pos);
}

double DoubleSpinBox::valueFromText(const QString &text) const
{
    return locale().toDouble(validator.regularExpression().match(text).captured());
}

QString DoubleSpinBox::textFromValue(double val) const
{
    if (multiple)
        return "";

    auto ans = locale().toString(val, 'f', decimals());

    for (int i = 0; i < decimals() - 1; i++)
        if (ans.endsWith("0"))
            ans.chop(1);

    if (ans.endsWith("0"))
        ans.chop(2);

    return ans;
}

void DoubleSpinBox::setMultipleValues()
{
    setValue(0, false);

    QDoubleSpinBox::setSuffix("");
    edit->setMultipleValues(multiple = true);
}

void DoubleSpinBox::setSuffix(const QString& suffix)
{
    QDoubleSpinBox::setSuffix(this->suffix = suffix);
}

void DoubleSpinBox::setMaximum(double max, bool spontaneous)
{
    if (value() > max)
        setValue(max, spontaneous);

    QDoubleSpinBox::setMaximum(max);
}

void DoubleSpinBox::setMinimum(double min, bool spontaneous)
{
    if (value() < min)
        setValue(min, spontaneous);

    QDoubleSpinBox::setMinimum(min);
}

#include <QStyle>

void DoubleSpinBox::focusInEvent(QFocusEvent *event)
{
    QDoubleSpinBox::focusInEvent(event);

    style()->unpolish(this);
    style()->polish(this);

    update();
}

void DoubleSpinBox::focusOutEvent(QFocusEvent *event)
{
    QDoubleSpinBox::focusOutEvent(event);

    style()->unpolish(this);
    style()->polish(this);

    update();
}
