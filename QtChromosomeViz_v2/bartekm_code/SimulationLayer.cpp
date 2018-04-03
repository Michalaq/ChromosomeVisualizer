#include "SimulationLayer.h"

SimulationLayer::SimulationLayer(const std::string & name)
    : name_(name)
    , frameCount_(0)
    , connectionCount_(0)
    , first(0), last(-1), stride(1)
{}

frameNumber_t SimulationLayer::getFrameCount() const
{
    return frameCount_ > first ? (frameCount_ - first) / stride : 0;
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

void SimulationLayer::setLayerId(int layerId)
{
    layerId_ = layerId;
}
