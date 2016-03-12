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
    int lowerBound;
    int upperBound;

    enum { NoHandle, LowerBoundHandle, UpperBoundHandle } activeHandle;

signals:
    void lowerBoundChanged(int);
    void upperBoundChanged(int);

public slots:
    void setLowerBound(int);
    void setUpperBound(int);
};

#endif // RANGESLIDER_H
