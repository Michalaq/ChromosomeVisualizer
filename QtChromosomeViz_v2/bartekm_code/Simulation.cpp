#include "Simulation.h"

Simulation::Simulation(const std::string &name)
    : name_(name)
    , frameCount_(0)
    , connectionCount_(-1)
{}

frameNumber_t Simulation::getFrameCount() const
{
    return frameCount_;
}

void Simulation::setSimulationName(const std::string &name)
{
    name_ = name;
}

const std::string & Simulation::getSimulationName() const
{
    return name_;
}

const int Simulation::getConnectionCount() const
{
    return connectionCount_;
}

TreeModel* Simulation::getModel()
{
    return model;
}
