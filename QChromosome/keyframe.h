#ifndef KEYFRAME_H
#define KEYFRAME_H

#include <QWidget>

namespace Ui {
    class Keyframes;
}

#include "interpolator.h"

class Keyframes : public QWidget
{
    Q_OBJECT
public:
    explicit Keyframes(QWidget *parent = 0);
    ~Keyframes();

    void setInterpolator(Interpolator* _ip);

signals:

public slots:
    void updateContents();

private:
    Ui::Keyframes *ui;
    Interpolator *ip;
};

#endif // KEYFRAME_H
