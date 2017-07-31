#include "spinbox.h"
#include <QLineEdit>

SpinBox::SpinBox(QWidget *parent) : QSpinBox(parent)
{
    setKeyboardTracking(false);

    lineEdit()->setValidator(Q_NULLPTR);

    re.setPattern("[+-]?\\d+");

    connect(this, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this] {
        setSpecialValueText("");
    });
}

SpinBox::~SpinBox()
{

}

void SpinBox::setValue(int val, bool spontaneous)
{
    bool b;

    if (!spontaneous)
        b = blockSignals(true);

    if (val > std::numeric_limits<int>::lowest())
    {
        setSpecialValueText("");
        QSpinBox::setValue(val);
    }
    else
    {
        setSpecialValueText("<< multiple values >>");
        QSpinBox::setValue(minimum());
    }

    if (!spontaneous)
        blockSignals(b);
}

QValidator::State SpinBox::validate(QString &input, int &) const
{
    return re.match(input).hasMatch() ? QValidator::Acceptable : QValidator::Intermediate;
}

int SpinBox::valueFromText(const QString &text) const
{
    return locale().toInt(re.match(text).captured());
}

#include <QStyle>

void SpinBox::focusInEvent(QFocusEvent *event)
{
    QSpinBox::focusInEvent(event);

    style()->unpolish(this);
    style()->polish(this);

    update();
}

void SpinBox::focusOutEvent(QFocusEvent *event)
{
    QSpinBox::focusOutEvent(event);

    style()->unpolish(this);
    style()->polish(this);

    update();
}
