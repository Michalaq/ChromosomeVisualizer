#ifndef PICKER_H
#define PICKER_H

#include "combobox.h"
#include <QVariant>

class Picker : public ComboBox
{
    Q_OBJECT
public:
    explicit Picker(QWidget *parent = 0);

    void setValue(const QVariant& c);
    void showAlphaChannel(bool c = true);

    QColor getColor() const;

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    QVariant color;
    bool alpha;

signals:
    void valueChanged(QColor);

public slots:
};

#endif // PICKER_H
