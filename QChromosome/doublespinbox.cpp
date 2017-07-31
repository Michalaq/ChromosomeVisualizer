#include "doublespinbox.h"
#include <QLineEdit>

DoubleSpinBox::DoubleSpinBox(QWidget *parent) : QDoubleSpinBox(parent)
{
    setKeyboardTracking(false);

    lineEdit()->setValidator(Q_NULLPTR);

    re.setPattern(QString("[+-]?\\d+(%1\\d+)?").arg(QRegularExpression::escape(locale().decimalPoint())));

    connect(this, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this] {
        setSpecialValueText("");
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

    if (val > std::numeric_limits<double>::lowest())
    {
        setSpecialValueText("");
        QDoubleSpinBox::setValue(val);
    }
    else
    {
        setSpecialValueText("<< multiple values >>");
        QDoubleSpinBox::setValue(minimum());
    }

    if (!spontaneous)
        blockSignals(b);
}

QValidator::State DoubleSpinBox::validate(QString &input, int &) const
{
    return re.match(input).hasMatch() ? QValidator::Acceptable : QValidator::Intermediate;
}

double DoubleSpinBox::valueFromText(const QString &text) const
{
    return locale().toDouble(re.match(text).captured());
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
