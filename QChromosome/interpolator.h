#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include <QObject>
#include <QMap>
#include <QPair>
#include <QVector3D>
#include <QDoubleSpinBox>
#include "spline.h"

class Interpolator : public QObject
{
    Q_OBJECT
public:
    explicit Interpolator(QObject *parent = 0);
    ~Interpolator();

    QMap<double, QPair<QVector3D,QVector3D>> keyframes;
    tk::spline _x, _y, _z, _h, _p, _b;

    void updateCurves();

    QMap<double, QPair<QVector3D,QVector3D>>::iterator frame;
    QMap<int,bool> locked;

    void setKey(QSpinBox* sb);
    void trackValues(QVector<QDoubleSpinBox *> sb);

    void setFrame(int frame);

    void setRecordingState(bool b);

    void selectKeyframe(int frame);
    int selectedKeyframe() const;

signals:
    void keyframeSelected();
    void interpolationChanged();

public slots:
    void recordKeyframe();

private:
    QSpinBox* key;
    QVector<QDoubleSpinBox*> keys;
    QMap<double, QVector<double>> values;
    QVector<tk::spline> splines;

    void updateSplines();

    int ignore;

    bool isRecording;

    QMap<double, QVector<double>>::iterator selectedFrame;
};

#endif // INTERPOLATOR_H
