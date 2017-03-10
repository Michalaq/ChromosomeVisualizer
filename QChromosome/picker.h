#ifndef PICKER_H
#define PICKER_H

#include "combobox.h"
#include <QVariant>

class Picker : public ComboBox
{
    Q_OBJECT
public:
    explicit Picker(QWidget *parent = 0);

    QColor value() const;
    void setValue(const QColor& c, bool spontaneous = true);

    void showAlphaChannel(bool c = true);

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    QColor color;
    bool alpha;

signals:
    void valueChanged(QColor);

public slots:
};

#endif // PICKER_H
