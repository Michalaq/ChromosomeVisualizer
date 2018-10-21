#ifndef BINSIMULATIONLAYER_H
#define BINSIMULATIONLAYER_H


#include "simulationlayer.h"

class BINSimulationLayer : public SimulationLayer
{
public:
    BINSimulationLayer(const QString& name, Session* s, int f = 0, int l = INT_MAX, int t = 1, bool b = true);
    ~BINSimulationLayer() override;

    void readEntry(int time, char* data, std::size_t stride, std::size_t pointer) override;

    int cacheHeaders(int time) override;
};

#endif // BINSIMULATIONLAYER_H
