#ifndef NULLSIMULATION_H
#define NULLSIMULATION_H

#include "Simulation.h"

class NullSimulation : public Simulation
{
public:
    NullSimulation();
    virtual std::shared_ptr<Frame> getFrame(frameNumber_t position) override;
};

#endif // NULLSIMULATION_H
