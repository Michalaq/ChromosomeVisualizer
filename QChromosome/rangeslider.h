#ifndef RANGESLIDER_H
#define RANGESLIDER_H

#include <QSlider>

class RangeSlider : public QSlider
{
    Q_OBJECT
public:
    explicit RangeSlider(QWidget *parent = 0);
    ~RangeSlider();

    QSize minimumSizeHint() const;

    int getLowerBound() const;
    int getUpperBound() const;

protected:
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    int lowerBound;
    int upperBound;

    int relativeLowerBound;
    int relativeUpperBound;

    enum { Normal, LeftHandleMoving, RightHandleMoving, IntervalMoving } state;

    void setBounds(int min, int max, bool spontaneous = true);

signals:
    void lowerBoundChanged(int);
    void upperBoundChanged(int);

public slots:
    void setLowerBound(int value, bool spontaneous = true);
    void setUpperBound(int value, bool spontaneous = true);

    void setMinimum(int min);
    void setMaximum(int max);
};

#endif // RANGESLIDER_H
