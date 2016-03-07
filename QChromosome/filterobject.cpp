#include "filterobject.h"

const QHash<QString, QPair<QEvent::Type, QEvent::Type> > FilterObject::pseudostate2event =
{
    { "hover", { QEvent::HoverEnter, QEvent::HoverLeave } },
    { "pressed", { QEvent::MouseButtonPress, QEvent::MouseButtonRelease } }
};

FilterObject::FilterObject(QObject *parent) : QObject(parent)
{
    if (auto tmp = qobject_cast<QWidget*>(parent))
        cacheProperties(tmp);
}

#include "draggable.h"
#include <QMouseEvent>

bool FilterObject::eventFilter(QObject *watched, QEvent *event)
{
    if (Draggable::pressedButton() != Qt::NoButton)
    {
        if (event->type() == QEvent::MouseButtonPress && event->spontaneous())
            return true;

        if (event->type() == QEvent::MouseButtonRelease && reinterpret_cast<QMouseEvent*>(event)->button() != Draggable::pressedButton())
            return true;
    }

    const auto enterEvent = qproperty_enter.find(QPair<QObject*, QEvent::Type>(watched, event->type()));

    if (enterEvent != qproperty_enter.end())
    {
        auto *backup = enterEvent.value().second;

        for (const auto& i : enterEvent.value().first)
        {
            backup->push_back({i.first, watched->property(i.first)});
            watched->setProperty(i.first, i.second);
        }
    }

    const auto leaveEvent = qproperty_leave.find(QPair<QObject*, QEvent::Type>(watched, event->type()));

    if (leaveEvent != qproperty_leave.end())
    {
        for (const auto& i : leaveEvent.value())
            watched->setProperty(i.first, i.second);

        leaveEvent.value().clear();
    }

    return QObject::eventFilter(watched, event);
}

#include <QRegularExpression>

void FilterObject::cacheProperties(QWidget *widget, QHash<QString, QHash<QString, QHash<QString, QVariant> > > cache)
{
    static const QRegularExpression selector("(?<class>\\w*)\\s*:(?<pseudostate>\\w+)\\s*{(?<stylesheet>[^{}]*)}");

    auto i = selector.globalMatch(widget->styleSheet());

    while (i.hasNext())
    {
        auto selector = i.next();

        auto& styleSheet = cache[selector.captured("class")][selector.captured("pseudostate")];

        static const QRegularExpression property("qproperty-(?<name>\\w+)\\s*:\\s*(?<value>\\S+)\\s*;");

        auto i = property.globalMatch(selector.captured("stylesheet"));

        while (i.hasNext())
        {
            auto property = i.next();

            styleSheet[property.captured("name")] = property.captured("value");
        }
    }

    auto styleSheet = cache[widget->metaObject()->className()];

    for (auto i = cache[""].cbegin(); i != cache[""].cend(); i++)
    {
        for (auto j = i.value().cbegin(); j != i.value().cend(); j++)
        {
            styleSheet[i.key()].insert(j.key(), j.value());
        }
    }

    cache.remove("");

    for (auto i = styleSheet.cbegin(); i != styleSheet.cend(); i++)
    {
        auto& pseudostate = qproperty_enter[QPair<QObject*, QEvent::Type>(widget, pseudostate2event[i.key()].first)];

        for (auto j = i.value().cbegin(); j != i.value().cend(); j++)
            pseudostate.first.push_back({ j.key().toUtf8(), j.value() });

        pseudostate.second = &qproperty_leave[QPair<QObject*, QEvent::Type>(widget, pseudostate2event[i.key()].second)];
    }

    for (auto child : widget->children())
        if (auto tmp = qobject_cast<QWidget*>(child))
            cacheProperties(tmp, cache);
}
