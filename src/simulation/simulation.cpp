#include "simulation.h"
#include "session.h"
#include "treemodel.h"

Simulation::Simulation(Session* s) :
    model(new TreeModel(s)),
    session(s)
{

}

Simulation::~Simulation()
{
    qDeleteAll(items);
    delete model;
}

void Simulation::readEntry(int time, char* data, std::size_t stride, std::size_t pointer)
{
    for (auto i : items)
        i->readEntry(time, data, stride, pointer);
}

int Simulation::cacheHeaders(int time)
{
    int ans = -1;

    for (auto i : items)
        ans = qMax(ans, i->cacheHeaders(time));

    return ans;
}

TreeModel* Simulation::getModel() const
{
    return model;
}

void Simulation::prepend(SimulationItem* value)
{
    items.prepend(value);
    model->prepend(value->getModel());
}

void Simulation::removeOne(SimulationItem* layer)
{
    items.removeOne(layer);
}

int Simulation::lastEntry() const
{
    int ans = 0;

    for (auto i : items)
        ans = qMax(ans, i->lastEntry());

    return ans;
}
