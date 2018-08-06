#include "spinbox.h"

SpinBox::SpinBox(QWidget *parent) :
    QSpinBox(parent),
    validator(QRegularExpression(QString("[+-]?\\d+")), this),
    edit(new LineEdit(this)),
    multiple(false)
{
    setKeyboardTracking(false);

    edit->setValidator(&validator);
    setLineEdit(edit);

    connect(this, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int val) {
        setValue(val, false);
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

    QSpinBox::setSuffix(suffix);
    edit->setMultipleValues(multiple = false);
    QSpinBox::setValue(val);

    if (!spontaneous)
        blockSignals(b);
}

QValidator::State SpinBox::validate(QString &input, int &pos) const
{
    return validator.validate(input, pos);
}

int SpinBox::valueFromText(const QString &text) const
{
    return locale().toInt(validator.regularExpression().match(text).captured());
}

QString SpinBox::textFromValue(int val) const
{
    if (multiple)
        return "";

    return locale().toString(val);
}

void SpinBox::setMultipleValues()
{
    setValue(0, false);

    QSpinBox::setSuffix("");
    edit->setMultipleValues(multiple = true);
}

void SpinBox::setSuffix(const QString& suffix)
{
    QSpinBox::setSuffix(this->suffix = suffix);
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
