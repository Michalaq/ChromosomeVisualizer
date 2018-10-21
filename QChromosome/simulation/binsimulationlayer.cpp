#include "binsimulationlayer.h"

BINSimulationLayer::BINSimulationLayer(const QString& name, Session* s, int f, int l, int t, bool b) :
    SimulationLayer(name, s, f, l, t)
{

}

BINSimulationLayer::~BINSimulationLayer()
{

}

void BINSimulationLayer::readEntry(int time, char* data, std::size_t stride, std::size_t pointer)
{

}

int BINSimulationLayer::cacheHeaders(int time)
{
    return -1;
}
