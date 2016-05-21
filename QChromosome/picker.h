#ifndef PICKER_H
#define PICKER_H

#include <QWidget>

class Picker : public QWidget
{
    Q_OBJECT
public:
    explicit Picker(QWidget *parent = 0);

    void setInitialColor(const QColor& c);

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    QColor color;

signals:
    void valueChanged(QColor);

public slots:
};

#endif // PICKER_H
