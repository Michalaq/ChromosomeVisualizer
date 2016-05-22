#ifndef PICKER_H
#define PICKER_H

#include <QWidget>
#include <QVariant>

class Picker : public QWidget
{
    Q_OBJECT
public:
    explicit Picker(QWidget *parent = 0);

    void setValue(const QVariant& c);

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    QVariant color;

signals:
    void valueChanged(QColor);

public slots:
};

#endif // PICKER_H
