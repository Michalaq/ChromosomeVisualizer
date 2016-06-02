#include "Simulation.h"

#include <QDebug>


Simulation::Simulation()
    : frameCount_(0),
      nextUnreadFrame_(0),
      model(new TreeModel(this))
{}

frameNumber_t Simulation::getFrameCount() const
{
    return nextUnreadFrame_ + 1;
}

std::shared_ptr<Frame> Simulation::getFrame(frameNumber_t position)
{
    Frame f = {
        0,
        0,
        std::vector<Atom>(),
        std::map<std::string, float>()
    };
    qDebug() << "GET";
    int count = 0;
    int i = 0;
    for (const auto & l : layerConcatenations_) {
        std::shared_ptr<Frame> f2 = l->getFrame(position);
        f.no = f2->no;
        f.step = f2->step;

        for (const auto& p : f2->functionValues) {
            f.functionValues[p.first + "_layer_" + std::to_string(i)] = p.second;
            qDebug() << (p.first + "_layer_" + std::to_string(i)).c_str();
        }

        for (const auto & atom : f2->atoms) {
            f.atoms.push_back(atom);
            f.atoms.back().id += count;
        }

        for (const auto & connection : f2->connectedRanges) {
            f.connectedRanges.push_back({ connection.first + count, connection.second + count });
        }

        count += f2->atoms.size();
        i++;
    }
    qDebug() << "END";
//    for (const auto& p : f.connectedRanges) {
//        std::cout << "AGGREGATED CONNECTION: " << p.first << ", " << p.second << std::endl;
//    }

    frameNumber_t nextFrame = getNextTime(position);
    if (nextUnreadFrame_ < nextFrame) {
        nextUnreadFrame_ = nextFrame;
        emit frameCountChanged(nextUnreadFrame_);
    }

    return std::make_shared<Frame>(f);
}

frameNumber_t Simulation::getNextTime(frameNumber_t time)
{
    frameNumber_t minimum = std::numeric_limits<frameNumber_t>::max();
    for (auto & concatenation : layerConcatenations_) {
        frameNumber_t localMinimum = concatenation->getNextTime(time);
        if (localMinimum != time)
            minimum = std::min(minimum, localMinimum);
    }

    if (minimum == std::numeric_limits<frameNumber_t>::max())
        return 0;

    return minimum;
}

frameNumber_t Simulation::getPreviousTime(frameNumber_t time)
{
    if (layerConcatenations_.empty())
        return 0;
    frameNumber_t maximum = std::numeric_limits<frameNumber_t>::min();
    for (auto & concatenation : layerConcatenations_) {
        frameNumber_t localMaximum = concatenation->getPreviousTime(time);
        if (localMaximum != time)
            maximum = std::max(maximum, localMaximum);
    }

    if (maximum == std::numeric_limits<frameNumber_t>::min())
        return 0;

    return maximum;
}

void Simulation::addSimulationLayerConcatenation(std::shared_ptr<SimulationLayerConcatenation> slc)
{
    const auto offset = getFrame(0)->atoms.size();
    layerConcatenations_.emplace_back(slc);

    connect(layerConcatenations_.back().get(), &SimulationLayerConcatenation::frameCountChanged,
            [this] (int frameCount) {
        if (frameCount_ < frameCount) {
            frameCount_ = frameCount;
            // emit frameCountChanged(frameCount_);
        }
    });

    int layerId = layerConcatenations_.size() - 1;
    slc->setLayerId(layerId);

    model->setupModelData(slc->getFrame(0)->atoms, layerConcatenations_.size(), offset);
}

std::shared_ptr<SimulationLayerConcatenation> Simulation::getSimulationLayerConcatenation(int i)
{
    return layerConcatenations_[i];
}

TreeModel* Simulation::getModel()
{
    return model;
}
