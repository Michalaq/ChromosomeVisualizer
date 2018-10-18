#include "simulation.h"
#include "session.h"

SimulationV2::SimulationV2(Session* s) :
    QVector<SimulationLayerV2*>(),
    session(s)
{

}

SimulationV2::~SimulationV2()
{
    qDeleteAll(*this);
}

void SimulationV2::loadEntry(int time)
{
    for (auto i : *this)
        i->loadEntry(time, session);
}

int SimulationV2::cacheHeaders(int limit)
{
    int ans = -1;

    for (auto i : *this)
        ans = qMax(ans, i->cacheHeaders(limit));

    return ans;
}
