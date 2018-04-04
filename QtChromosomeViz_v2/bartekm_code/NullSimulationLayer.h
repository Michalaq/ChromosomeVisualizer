#ifndef NULLSIMULATION_H
#define NULLSIMULATION_H

#include "SimulationLayer.h"

class NullSimulationLayer : public UntransformedSimulationLayer
{
public:
    NullSimulationLayer();
    virtual ~NullSimulationLayer() override;
    virtual std::shared_ptr<Frame> getFrameById(frameNumber_t position) = 0;
    virtual std::shared_ptr<Frame> getFrame(frameNumber_t time) = 0;
    virtual frameNumber_t getNextTime(frameNumber_t time) = 0;
    virtual frameNumber_t getPreviousTime(frameNumber_t time) = 0;
    virtual bool reachedEndOfFile() const override;
};

#endif // NULLSIMULATION_H
