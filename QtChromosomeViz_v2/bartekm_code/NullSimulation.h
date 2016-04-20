#ifndef NULLSIMULATION_H
#define NULLSIMULATION_H

#include "SimulationLayer.h"

class NullSimulation : public SimulationLayer
{
public:
    NullSimulation();
    virtual std::shared_ptr<Frame> getFrame(frameNumber_t position) override;
};

#endif // NULLSIMULATION_H
