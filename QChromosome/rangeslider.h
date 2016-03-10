#ifndef RANGESLIDER_H
#define RANGESLIDER_H

#include <QSlider>

class RangeSlider : public QSlider
{
    Q_OBJECT
public:
    explicit RangeSlider(QWidget *parent = 0);

    QSize minimumSizeHint() const;

protected:
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    QPair<int, int> range;
    int *handle;

signals:

public slots:
};

#endif // RANGESLIDER_H
