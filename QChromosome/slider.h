#ifndef SLIDER_H
#define SLIDER_H

#include <QSlider>

class Slider : public QSlider
{
    Q_OBJECT
public:
    explicit Slider(QWidget *parent = 0);

    void setMinimum(int min);
    void setMaximum(int max);

private:
    int softMinimum;
    int softMaximum;

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *event);

signals:

public slots:
    void setSoftMinimum(int min);
    void setSoftMaximum(int max);
};

#endif // SLIDER_H
