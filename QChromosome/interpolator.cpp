#include "interpolator.h"

Interpolator::Interpolator(QObject *parent) : QObject(parent), ignore(0)
{
    selectedFrame = values.end();
}

Interpolator::~Interpolator()
{

}

#include <QVector>

void Interpolator::recordKeyframe()
{
    if (ignore)
        ignore--;
    else
    {
        int n = tracked.size();

        QVector<double> v(n);

        for (int i = 0; i < n; i++)
            v[i] = tracked[i]->value();

        values.insert(key->value(), v);

        updateSplines();
    }
}

void Interpolator::setKey(QSpinBox *sb)
{
    key = sb;
}

void Interpolator::trackValues(QVector<QDoubleSpinBox *> sb)
{
    tracked.swap(sb);

    splines.resize(tracked.size());

    for (auto& s : splines)
        s.set_boundary(tk::spline::first_deriv, 0, tk::spline::first_deriv, 0, true);
}

void Interpolator::setFrame(int frame)
{
    if (values.uniqueKeys().size() < 2)
        return;

    ignore = isRecording ? 2 : 0;

    // TODO hack, omijający nadpisywanie przez modelViewChanged
    for (int i = 0; i < 3; i++)
        tracked[i]->setValue(splines[i](frame));

    tracked[0]->editingFinished();

    for (int i = 3; i < 6; i++)
        tracked[i]->setValue(splines[i](frame));

    tracked[3]->editingFinished();
}

void Interpolator::setRecordingState(bool b)
{
    isRecording = b;
    if (b) ignore = 1; //TODO hack, żeby ominąć sygnał wysłany przez connectNotify
}

void Interpolator::selectKeyframe(int frame)
{
    selectedFrame = values.find(frame);

    emit selectionChanged();
}

int Interpolator::selectedKeyframe() const
{
    return selectedFrame == values.end() ? -1 : selectedFrame.key();
}

QList<double> Interpolator::keys() const
{
    return values.uniqueKeys();
}

void Interpolator::changeKey(int frame, bool hard)
{
    if (selectedFrame == values.end() || (selectedFrame.key() == frame && !hard) || lockedKeys.contains(selectedFrame.key()))
        return;

    auto v = selectedFrame.value();

    values.erase(selectedFrame);

    selectedFrame = hard ? values.insert(frame, v) : values.insertMulti(frame, v);

    updateSplines();

    emit selectionChanged();
}

void Interpolator::lockKey(bool c)
{
    if (selectedFrame != values.end())
    {
        if (c)
            lockedKeys.insert(selectedFrame.key());
        else
            lockedKeys.remove(selectedFrame.key());
    }
}

bool Interpolator::isKeyLocked(int frame) const
{
    return lockedKeys.contains(frame);
}

void Interpolator::deleteKeyrame()
{
    if (selectedFrame != values.end())
    {
        values.erase(selectedFrame);
        selectedFrame = values.end();

        updateSplines();

        emit selectionChanged();
    }
}

void Interpolator::updateSplines()
{
    std::vector<double> d = values.uniqueKeys().toVector().toStdVector();

    int n = d.size();

    if (n < 2)
    {
        emit interpolationChanged();
        return;
    }

    int m = tracked.size();

    auto v = new std::vector<double>[m];

    for (int i = 0; i < n; i++)
    {
        auto& t = values[d[i]];

        for (int j = 0; j < m; j++)
            v[j].push_back(t[j]);
    }

    for (int i = 0; i < m; i++)
        splines[i].set_points(d, v[i]);

    emit interpolationChanged();
}
