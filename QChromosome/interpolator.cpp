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

    if (!splines.isEmpty())
    {
        SplineKeyframe f;

        for (auto i = splines.begin(); i != splines.end(); i++)
            f.insert(i.key(), i.value()(currentFrame));

        loadFrame(f);
    }
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

void SplineInterpolator::writePOVSpline(QTextStream &stream, std::function<void(QTextStream &, const SplineKeyframe &)> f) const
{
    stream << "spline { cubic_spline\n";

    {
        auto j = keyframes.begin();
        auto i = j + 1;

        stream << 2 * j.key() - i.key() << ", ";
        f(stream, i.value());
        stream << "\n";
    }

    for (auto i = keyframes.begin(); i != keyframes.end(); i++)
    {
        stream << i.key() << ", ";
        f(stream, i.value());
        stream << "\n";
    }

    {
        auto j = keyframes.end() - 1;
        auto i = j - 1;

        stream << 2 * j.key() - i.key() << ", ";
        f(stream, i.value());
        stream << "\n";
    }

    stream << "}\n";
}

int SplineInterpolator::count() const
{
    return keyframes.size();
}
