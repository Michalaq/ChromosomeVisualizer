#include "SimulationLayer.h"

SimulationLayer::SimulationLayer(const std::string & name)
    : name_(name)
    , frameCount_(0)
    , connectionCount_(-1)
{}

frameNumber_t SimulationLayer::getFrameCount() const
{
    return frameCount_;
}

void SimulationLayer::setSimulationLayerName(const std::string &name)
{
    name_ = name;
}

const std::string & SimulationLayer::getSimulationLayerName() const
{
    return name_;
}

const int SimulationLayer::getConnectionCount() const
{
    return connectionCount_;
}
