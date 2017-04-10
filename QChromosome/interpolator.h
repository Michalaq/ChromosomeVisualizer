#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include <QMap>
#include <QSet>
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

    void setKey(QSpinBox* sb);

    void trackValues(QVector<QDoubleSpinBox *> sb);

    void setFrame(int frame);

    void setRecordingState(bool b);

    void selectKeyframe(int frame);
    int selectedKeyframe() const;

    QList<double> keys() const;

    void changeKey(int frame, bool hard = true);

    void lockKey(bool c = true);
    bool isKeyLocked() const;

    void lockValue(bool c = true);
    bool isValueLocked(int frame) const;

signals:
    void selectionChanged();
    void interpolationChanged();

public slots:
    void recordKeyframe();
    void deleteKeyrame();

private:
    QSpinBox* key;
    QVector<QDoubleSpinBox*> tracked;
    QMap<double, QPair<QVector<double>, QPair<bool, bool>>> values;
    QVector<tk::spline> splines;

    void updateSplines();

    int ignore;

    bool isRecording;

    QMap<double, QPair<QVector<double>, QPair<bool, bool>>>::iterator selectedFrame;
};

#endif // INTERPOLATOR_H