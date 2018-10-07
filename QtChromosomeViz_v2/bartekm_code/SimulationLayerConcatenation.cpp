#include <limits>
#include <cassert>
#include "SimulationLayerConcatenation.h"

SimulationLayerConcatenation::SimulationLayerConcatenation()
    : QObject()
    , frameCount_(0)
{
    aggregatedFrameCounts_.push_back(0);
}

SimulationLayerConcatenation::SimulationLayerConcatenation(std::shared_ptr<SimulationLayer> sl)
    : SimulationLayerConcatenation()
{
    appendSimulationLayer(std::move(sl));
}

SimulationLayerConcatenation::~SimulationLayerConcatenation()
{}

frameNumber_t SimulationLayerConcatenation::getLayerBaseFrameNumber(int layerID)
{
    if (aggregatedFrameCounts_.size() > layerID)
        return aggregatedFrameCounts_[layerID];

    frameNumber_t previousCount = getLayerBaseFrameNumber(layerID - 1);
    auto & layer = layers_[layerID - 1];

    // Force calculation of the number of frames for the layer
    // (current implementation of layers returns the last frame if EOF is encountered)
    if (!layer->reachedEndOfFile())
        layer->getFrame(std::numeric_limits<frameNumber_t>::max());

    aggregatedFrameCounts_.push_back(previousCount + layer->getFrameCount());
    assert(aggregatedFrameCounts_.size() == layerID + 1);
    return aggregatedFrameCounts_[layerID];
}

frameNumber_t SimulationLayerConcatenation::getFrameCount() const
{
    return frameCount_;
}

const std::string & SimulationLayerConcatenation::getSimulationLayerConcatenationName() const
{
    static const std::string empty = "";
    return layers_.empty() ? empty : layers_.front()->getSimulationLayerName();
}

void SimulationLayerConcatenation::appendSimulationLayer(std::shared_ptr<SimulationLayer> sl)
{
    // TODO: Check compatibility with other frames
    const int layerID = layers_.size();
    layers_.emplace_back(std::move(sl));
    connect(layers_.back().get(), &SimulationLayer::frameCountChanged,
            [this, layerID] (int frameCount) {
        const frameNumber_t base = getLayerBaseFrameNumber(layerID);
        if (frameCount_ < base + frameCount) {
            frameCount_ = base + frameCount;
            emit frameCountChanged(frameCount_);
        }
    });
}

int SimulationLayerConcatenation::getConnectionCount() const
{
    if (layers_.empty())
        return 0;
    return layers_.front()->getConnectionCount();
}

std::shared_ptr<Frame> SimulationLayerConcatenation::getFrame(frameNumber_t position)
{
    if (layers_.empty())
        return std::make_shared<Frame>();

    if (position < 0)
        position = 0;

    for (const auto & layer : layers_) {
        std::shared_ptr<Frame> frame;
        if (!layer->reachedEndOfFile()) {
            // The frame we look for might be in this layer, so let's try
            frame = layer->getFrame(position);

            if (position < layer->getFrameCount()) {
                // Yep, found it
                return frame;
            } else {
                assert(layer->reachedEndOfFile());
            }
        } else if (position < layer->getFrameCount()) {
            return layer->getFrame(position);
        }

        // Try the next one
        position -= layer->getFrameCount();
    }

    return layers_.back()->getFrame(layers_.back()->getFrameCount());
}

frameNumber_t SimulationLayerConcatenation::getNextTime(frameNumber_t time)
{
    if (time < 0)
        time = 0;

    frameNumber_t framesSkipped = 0;

    for (const auto & layer : layers_) {
        if (time < layer->getFrameCount()) {
            frameNumber_t nextTime = layer->getNextTime(time);
            if (nextTime != time)
                return framesSkipped + nextTime;
        }

        // Try the next one
        time -= layer->getFrameCount();
        framesSkipped += layer->getFrameCount();
    }

    return framesSkipped - 1;
}

frameNumber_t SimulationLayerConcatenation::getPreviousTime(frameNumber_t time)
{
    const frameNumber_t oldTime = time;
    if (time < 0)
        return 0;

    frameNumber_t framesSkipped = 0;

    for (const auto & layer : layers_) {
        frameNumber_t count = layer->getFrameCount();
        if (time == count && layer->reachedEndOfFile())
            return framesSkipped + count - 1;
        if (time < count)
            return framesSkipped + layer->getPreviousTime(time);

        // Try the next one
        time -= layer->getFrameCount();
        framesSkipped += layer->getFrameCount();
    }

    return oldTime - time - 1;
}

#include <QJsonArray>
#include <QJsonObject>

void SimulationLayerConcatenation::read(const QJsonArray &json)
{
    for (auto i : json)
        appendSimulationLayer(SimulationLayer::read(i.toObject()));
}

void SimulationLayerConcatenation::write(QJsonArray &json) const
{
    for (auto i : layers_)
    {
        QJsonObject layer;
        i->write(layer);
        json.append(layer);
    }
}
