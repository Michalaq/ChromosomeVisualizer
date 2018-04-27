#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include "draggable.h"
#include "spline.h"
#include <QMap>
#include <QSet>
#include <QItemSelectionModel>
#include <functional>
#include <QTextStream>

class SplineKeyframe
{
public:
    QMap<QString, double>::const_iterator constFind(const QString& key) const;
    QMap<QString, double>::const_iterator constEnd() const;
    QMap<QString, double>::iterator insert(const QString& key, double value);
    double value(const QString& key, double defaultValue = double()) const;

    void lockTime(bool b = true);
    void lockValue(bool b = true);

    bool timeLocked() const;
    bool valueLocked() const;

    void read(const QJsonObject& json);
    void write(QJsonObject &json) const;

private:
    QMap<QString, double> values;
    bool _timeLocked = false;
    bool _valueLocked = false;
};

class SplineInterpolator : public Draggable
{
    Q_OBJECT

public:
    SplineInterpolator(const QStringList& p, QWidget *parent = 0);
    virtual ~SplineInterpolator();

    /* changes key of selected frames */
    void adjustKeys(int delta);

    void setKey(int key);
    void lockTime(bool b = true);
    void lockValue(bool b = true);
    QMap<int, SplineKeyframe>::const_iterator selectedFrame() const;
    QMap<int, SplineKeyframe>::const_iterator constEnd() const;

    /* captures current state */
    void captureFrame();

    /* returns true if key is selected */
    bool isSelected(int key) const;

    /* returns a const iterator pointing to the key */
    QMap<int, SplineKeyframe>::key_iterator keyBegin() const;
    QMap<int, SplineKeyframe>::key_iterator keyEnd() const;

    /* removes selected keys */
    void remove();

    /* selects key using the specified command */
    void select(int key, QItemSelectionModel::SelectionFlags command);

    /* sets current frame for all interpolated objects */
    static void setFrame(int frame);

    /* creates frame from current state */
    virtual SplineKeyframe saveFrame() const = 0;

    /* restores current state from frame */
    virtual void loadFrame(const SplineKeyframe& frame) = 0;

    virtual void read(const QJsonArray& json);
    virtual void write(QJsonArray &json) const;

    void writePOVSpline(QTextStream &stream, std::function<void(QTextStream &, const SplineKeyframe &)> f) const;

    int count() const;

signals:
    void interpolationChanged();
    void selectionChanged();

private:
    static int currentFrame;
    bool needsUpdate;

    const QStringList header;
    QMap<QString, tk::spline> splines;
    QMap<int, SplineKeyframe> keyframes;
    QSet<int> selection;

    void updateFrame();

    static QSet<SplineInterpolator*> interpolators;
};

#endif // INTERPOLATOR_H
