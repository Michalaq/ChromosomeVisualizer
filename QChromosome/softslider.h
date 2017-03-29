#ifndef SOFTSLIDER_H
#define SOFTSLIDER_H

#include <QSlider>

class SoftSlider : public QSlider
{
    Q_OBJECT
public:
    explicit SoftSlider(QWidget *parent = 0);

    void setMinimum(int min);
    void setMaximum(int max);
    void setRange(int min, int max);

    int tickSpan(qreal minsep) const;
    int tickSpan(qreal minsep, int width) const;

    static qreal tickSpan(qreal min, qreal max, qreal space, qreal minsep);

protected:
    int softMinimum;
    int softMaximum;

signals:

public slots:
    void setSoftMinimum(int min);
    void setSoftMaximum(int max);
};

#endif // SOFTSLIDER_H
