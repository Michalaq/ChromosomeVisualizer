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

protected:
    int softMinimum;
    int softMaximum;

signals:

public slots:
    void setSoftMinimum(int min);
    void setSoftMaximum(int max);
};

#endif // SOFTSLIDER_H
