#include "simulation.h"
#include "session.h"
#include "treemodel.h"

SimulationV2::SimulationV2(Session* s) :
    QVector<SimulationLayerV2*>(),
    model(new TreeModel(s))
{

}

SimulationV2::~SimulationV2()
{
    qDeleteAll(*this);
    delete model;
}

void SimulationV2::loadEntry(int time)
{
    for (auto i : *this)
        i->loadEntry(time);
}

int SimulationV2::cacheHeaders(int time)
{
    int ans = -1;

    for (auto i : *this)
        ans = qMax(ans, i->cacheHeaders(time));

    return ans;
}

TreeModel* SimulationV2::getModel() const
{
    return model;
}

void SimulationV2::prepend(SimulationLayerV2* value)
{
    QVector<SimulationLayerV2*>::prepend(value);

    model->prepend(value->getModel());
}
