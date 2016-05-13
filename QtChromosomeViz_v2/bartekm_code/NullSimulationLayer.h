#ifndef NULLSIMULATION_H
#define NULLSIMULATION_H

#include "SimulationLayer.h"

class NullSimulationLayer : public SimulationLayer
{
public:
    NullSimulationLayer();
    virtual ~NullSimulationLayer() override;
    virtual std::shared_ptr<Frame> getFrame(frameNumber_t position) override;
};

#endif // NULLSIMULATION_H
