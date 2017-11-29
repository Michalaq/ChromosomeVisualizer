#include "interpolator.h"

QMap<QString, double>::const_iterator SplineKeyframe::constFind(const QString& key) const
{
    return values.constFind(key);
}

QMap<QString, double>::const_iterator SplineKeyframe::constEnd() const
{
    return values.constEnd();
}

QMap<QString, double>::iterator SplineKeyframe::insert(const QString& key, double value)
{
    return values.insert(key, value);
}

double SplineKeyframe::value(const QString& key, double defaultValue) const
{
    return values.value(key, defaultValue);
}

void SplineKeyframe::lockTime(bool b)
{
    _timeLocked = b;
}

void SplineKeyframe::lockValue(bool b)
{
    _valueLocked = b;
}

bool SplineKeyframe::timeLocked() const
{
    return _timeLocked;
}

bool SplineKeyframe::valueLocked() const
{
    return _valueLocked;
}

#include <QJsonObject>

void SplineKeyframe::read(const QJsonObject &json)
{
    const QJsonObject keyValues = json["Key values"].toObject();

    for (auto i = keyValues.begin(); i != keyValues.end(); i++)
        values[i.key()] = i.value().toDouble();

    _timeLocked = json["Lock time"].toBool();
    _valueLocked = json["Lock value"].toBool();
}

void SplineKeyframe::write(QJsonObject &json) const
{
    QJsonObject keyValues;

    for (auto i = values.begin(); i != values.end(); i++)
        keyValues[i.key()] = i.value();

    json["Key values"] = keyValues;
    json["Lock time"] = _timeLocked;
    json["Lock value"] = _valueLocked;
}

int SplineInterpolator::currentFrame = 0;

QSet<SplineInterpolator*> SplineInterpolator::interpolators;

SplineInterpolator::SplineInterpolator(const QStringList &p, QWidget *parent) :
    Draggable(parent),
    header(p),
    needsUpdate(false)
{
    interpolators.insert(this);
}

SplineInterpolator::~SplineInterpolator()
{
    interpolators.remove(this);
}

void SplineInterpolator::adjustKeys(int delta)
{
    if (delta == 0)
        return;

    QSet<int> tmp;

    if (delta < 0)
        for (auto i = selection.begin(); i != selection.end(); i++)
        {
            if (!keyframes[*i].timeLocked())
            {
                keyframes.insert(*i + delta, keyframes.take(*i));
                tmp.insert(*i + delta);
            }
            else
                tmp.insert(*i);
        }
    else
        for (auto i = selection.rbegin(); i != selection.rend(); i++)
        {
            if (!keyframes[*i].timeLocked())
            {
                keyframes.insert(*i + delta, keyframes.take(*i));
                tmp.insert(*i + delta);
            }
            else
                tmp.insert(*i);
        }

    selection.swap(tmp);

    needsUpdate = true;
    updateFrame();

    emit selectionChanged();
}

void SplineInterpolator::setKey(int key)
{
    QSet<int> tmp;

    for (auto i = selection.begin(); i != selection.end(); i++)
    {
        if (!keyframes[*i].timeLocked())
        {
            keyframes.insert(key, keyframes.take(*i));
            tmp.insert(key);
        }
        else
            tmp.insert(*i);
    }

    selection.swap(tmp);

    needsUpdate = true;
    updateFrame();

    emit selectionChanged();
}

void SplineInterpolator::setFrame(int frame)
{
    currentFrame = frame;

    for (auto i : interpolators)
        i->updateFrame();
}

void SplineInterpolator::lockTime(bool b)
{
    for (auto i = selection.begin(); i != selection.end(); i++)
        keyframes[*i].lockTime(b);
}

void SplineInterpolator::lockValue(bool b)
{
    for (auto i = selection.begin(); i != selection.end(); i++)
        keyframes[*i].lockValue(b);
}

QMap<int, SplineKeyframe>::const_iterator SplineInterpolator::selectedFrame() const
{
    return selection.isEmpty() ? keyframes.constEnd() : keyframes.find(*selection.constBegin());
}

QMap<int, SplineKeyframe>::const_iterator SplineInterpolator::constEnd() const
{
    return keyframes.constEnd();
}

void SplineInterpolator::captureFrame()
{
    if (!keyframes[currentFrame].valueLocked())
    {
        keyframes.insert(currentFrame, saveFrame());
        needsUpdate = true;
    }
}

QMap<int, SplineKeyframe>::key_iterator SplineInterpolator::keyBegin() const
{
    return keyframes.keyBegin();
}

QMap<int, SplineKeyframe>::key_iterator SplineInterpolator::keyEnd() const
{
    return keyframes.keyEnd();
}

bool SplineInterpolator::isSelected(int key) const
{
    return selection.contains(key);
}

void SplineInterpolator::remove()
{
    for (int key : selection)
        keyframes.remove(key);

    selection.clear();

    needsUpdate = true;
    updateFrame();
}

void SplineInterpolator::select(int key, QItemSelectionModel::SelectionFlags command)
{
    if (command & QItemSelectionModel::Clear)
        selection.clear();

    if (keyframes.contains(key))
    {
        if (command & QItemSelectionModel::Select)
            selection.insert(key);

        if (command & QItemSelectionModel::Deselect)
            selection.remove(key);
    }

    emit selectionChanged();
}

void SplineInterpolator::updateFrame()
{
    if (needsUpdate)
    {
        splines.clear();

        for (auto& s : header)
        {
            std::vector<double> k;
            std::vector<double> v;

            for (auto i = keyframes.begin(); i != keyframes.end(); i++)
            {
                auto j = i.value().constFind(s);

                if (j != i.value().constEnd())
                {
                    k.push_back(i.key());
                    v.push_back(j.value());
                }
            }

            if (k.size() > 1)
            {
                auto& spline = splines[s];
                spline.set_boundary(tk::spline::first_deriv, 0, tk::spline::first_deriv, 0, true);
                spline.set_points(k, v);
            }
        }

        needsUpdate = false;

        emit interpolationChanged();
    }

    SplineKeyframe f;

    for (auto i = splines.begin(); i != splines.end(); i++)
        f.insert(i.key(), i.value()(currentFrame));

    loadFrame(f);
}

#include <QJsonArray>

void SplineInterpolator::read(const QJsonArray &json)
{
    for (auto i : json)
    {
        const QJsonObject keyProperties = i.toObject();

        keyframes[keyProperties["Key time"].toInt()].read(keyProperties);
    }

    needsUpdate = true;
    updateFrame();
}

void SplineInterpolator::write(QJsonArray &json) const
{
    for (auto i = keyframes.begin(); i != keyframes.end(); i++)
    {
        QJsonObject keyProperties;
        keyProperties["Key time"] = i.key();

        i.value().write(keyProperties);

        json.append(keyProperties);
    }
}











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
        if (!isValueLocked(key->value()))
        {
            int n = tracked.size();

            QVector<double> v(n);

            for (int i = 0; i < n; i++)
                v[i] = tracked[i]->value();

            values.insert(key->value(), {v, {false, false}});

            updateSplines();
        }
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

    ignore = isRecording ? 3 : 0;

    // TODO hack, omijający nadpisywanie przez modelViewChanged
    for (int i = 0; i < 3; i++)
        tracked[i]->setValue(splines[i](frame));

    tracked[0]->editingFinished();

    for (int i = 3; i < 6; i++)
        tracked[i]->setValue(splines[i](frame));

    tracked[3]->editingFinished();

    for (int i = 6; i < 9; i++)
        tracked[i]->setValue(splines[i](frame));

    tracked[6]->editingFinished();
}

void Interpolator::setRecordingState(bool b)
{
    isRecording = b;
    if (b) ignore = 2; //TODO hack, żeby ominąć sygnał wysłany przez connectNotify
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
    if (selectedFrame == values.end() || (selectedFrame.key() == frame && !hard) || isKeyLocked())
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
        selectedFrame->second.first = c;
}

bool Interpolator::isKeyLocked() const
{
    return selectedFrame != values.end() && selectedFrame->second.first;
}

void Interpolator::lockValue(bool c)
{
    if (selectedFrame != values.end())
        selectedFrame->second.second = c;
}

bool Interpolator::isValueLocked(int frame) const
{
    auto f = values.find(frame);
    return f != values.end() && f->second.second;
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
        auto& t = values[d[i]].first;

        for (int j = 0; j < m; j++)
            v[j].push_back(t[j]);
    }

    for (int i = 0; i < m; i++)
        splines[i].set_points(d, v[i]);

    emit interpolationChanged();
}

#include <QJsonObject>
#include <QJsonArray>

void Interpolator::read(const QJsonArray &json)
{
    for (auto i : json)
    {
        QJsonObject keyProperties = i.toObject();

        QVector<double> keyValues;
        for (auto v : keyProperties["Key values"].toArray())
            keyValues.append(v.toDouble());

        values[keyProperties["Key time"].toDouble()] = { keyValues, { keyProperties["Lock time"].toBool(), keyProperties["Lock value"].toBool() } };
    }

    updateSplines();
}

void Interpolator::write(QJsonArray &json) const
{
    for (auto i = values.begin(); i != values.end(); i++)
    {
        QJsonObject keyProperties;
        keyProperties["Key time"] = i.key();

        QJsonArray keyValues;
        for (auto v : i.value().first)
            keyValues.append(v);
        keyProperties["Key values"] = keyValues;

        keyProperties["Lock time"] = i.value().second.first;
        keyProperties["Lock value"] = i.value().second.second;
        json.append(keyProperties);
    }
}

#include <fstream>

std::ostream &Interpolator::operator<<(std::ostream &stream) const
{
    stream << "#declare MySplinePos = \nspline {\ncubic_spline\n";

    {
        auto j = values.begin();
        auto i = j + 1;

        auto t = 2 * j.key() - i.key();
        auto& v = i.value().first;

        stream << t << ", <" << -v[3] << ", " << v[4] << ", " << v[5] << ">\n";
    }

    for (auto i = values.begin(); i != values.end(); i++)
    {
        auto& t = i.key();
        auto& v = i.value().first;

        stream << t << ", <" << -v[3] << ", " << v[4] << ", " << v[5] << ">\n";
    }

    {
        auto j = values.end() - 1;
        auto i = j - 1;

        auto t = 2 * j.key() - i.key();
        auto& v = i.value().first;

        stream << t << ", <" << -v[3] << ", " << v[4] << ", " << v[5] << ">\n";
    }

    stream << "}\n";

    stream << "#declare MySplineAng = \nspline {\ncubic_spline\n";

    {
        auto j = values.begin();
        auto i = j + 1;

        auto t = 2 * j.key() - i.key();
        auto& v = i.value().first;

        stream << t << ", <" << -v[7] << ", " << v[6] << ", " << v[8] << ">\n";
    }

    for (auto i = values.begin(); i != values.end(); i++)
    {
        auto& t = i.key();
        auto& v = i.value().first;

        stream << t << ", <" << -v[7] << ", " << v[6] << ", " << v[8] << ">\n";
    }

    {
        auto j = values.end() - 1;
        auto i = j - 1;

        auto t = 2 * j.key() - i.key();
        auto& v = i.value().first;

        stream << t << ", <" << -v[7] << ", " << v[6] << ", " << v[8] << ">\n";
    }

    stream << "}\n";

    return stream;
}

std::ostream &operator<<(std::ostream &stream, const Interpolator &ip)
{
    return ip << stream;
}
