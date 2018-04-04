#include "SimulationLayer.h"

UntransformedSimulationLayer::UntransformedSimulationLayer(const std::string & name)
    : name_(name)
    , frameCount_(0)
    , connectionCount_(0)
{}

frameNumber_t UntransformedSimulationLayer::getFrameCount() const
{
    return frameCount_;
}

void UntransformedSimulationLayer::setSimulationLayerName(const std::string &name)
{
    name_ = name;
}

const std::string & UntransformedSimulationLayer::getSimulationLayerName() const
{
    return name_;
}

const int UntransformedSimulationLayer::getConnectionCount() const
{
    return connectionCount_;
}

void UntransformedSimulationLayer::setLayerId(int layerId)
{
    layerId_ = layerId;
}

SimulationLayer::SimulationLayer(std::shared_ptr<UntransformedSimulationLayer> l)
    : layer(l)
    , first(0), last(-1), stride(1)
{
    connect(layer.get(), &UntransformedSimulationLayer::frameCountChanged, [this](frameNumber_t frameCount) {
        auto last_ = last < first ? std::numeric_limits<frameNumber_t>::max() - 1 : last;
        auto frameCount_ = std::min(frameCount, last_ + 1);

        emit frameCountChanged((frameCount_ - first + stride - 1) / stride);
    });
}

frameNumber_t SimulationLayer::getFrameCount() const
{
    auto last_ = last < first ? std::numeric_limits<frameNumber_t>::max() - 1 : last;
    auto frameCount_ = std::min(layer->getFrameCount(), last_ + 1);

    return (frameCount_ - first + stride - 1) / stride;
}

void SimulationLayer::setSimulationLayerName(const std::string & name)
{
    layer->setSimulationLayerName(name);
}

const std::string & SimulationLayer::getSimulationLayerName() const
{
    return layer->getSimulationLayerName();
}

const int SimulationLayer::getConnectionCount() const
{
    return layer->getConnectionCount();
}

void SimulationLayer::setLayerId(int layerId)
{
    layer->setLayerId(layerId);
}

std::shared_ptr<Frame> SimulationLayer::getFrame(frameNumber_t time)
{
    auto time_ = first + stride * time;
    auto last_ = last < first ? std::numeric_limits<frameNumber_t>::max() - 1 : last;

    return time_ <= last_ ? layer->getFrame(time_) : layer->getFrame(last_);
}

frameNumber_t SimulationLayer::getNextTime(frameNumber_t time)
{
    auto time_ = layer->getNextTime(first + stride * time);
    auto last_ = last < first ? std::numeric_limits<frameNumber_t>::max() - 1 : last;

    return time_ <= last_ ? (time_ - first + stride - 1) / stride : (last_ - first) / stride;
}

frameNumber_t SimulationLayer::getPreviousTime(frameNumber_t time)
{
    auto time_ = layer->getPreviousTime(first + stride * time);

    return time_ >= first ? (time_ - first) / stride : 0;
}

bool SimulationLayer::reachedEndOfFile() const
{
    auto last_ = last < first ? std::numeric_limits<frameNumber_t>::max() - 1 : last;

    return layer->reachedEndOfFile() || layer->getFrameCount() > last_;
}
