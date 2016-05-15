#include "Simulation.h"

Simulation::Simulation()
    : frameCount_(0)
{}

frameNumber_t Simulation::getFrameCount() const
{
    return frameCount_;
}

std::shared_ptr<Frame> Simulation::getFrame(frameNumber_t position)
{
    Frame f = {
        0,
        0,
        std::vector<Atom>(),
        std::map<std::string, float>()
    };
    int count = 0;
    for (const auto & l : layerConcatenations_) {
        std::shared_ptr<Frame> f2 = l->getFrame(position);
        f.no = f2->no;
        f.step = f2->step;
        f.functionValues = f2->functionValues;

        for (const auto & atom : f2->atoms) {
            f.atoms.push_back(atom);
            f.atoms.back().id += count;
        }

        for (const auto & connection : f2->connectedRanges) {
            f.connectedRanges.push_back({ connection.first + count, connection.second + count });
        }

        count += f2->atoms.size();
    }
    for (const auto& p : f.connectedRanges) {
        std::cout << "AGGREGATED CONNECTION: " << p.first << ", " << p.second << std::endl;
    }
    return std::make_shared<Frame>(f);
}

void Simulation::addSimulationLayerConcatenation(std::shared_ptr<SimulationLayerConcatenation> slc)
{
    layerConcatenations_.emplace_back(std::move(slc));
    connect(layerConcatenations_.back().get(), &SimulationLayerConcatenation::frameCountChanged,
            [this] (int frameCount) {
        if (frameCount_ < frameCount) {
            frameCount_ = frameCount;
            emit frameCountChanged(frameCount_);
        }
    });
}

std::shared_ptr<SimulationLayerConcatenation> Simulation::getSimulationLayerConcatenation(int i)
{
    return layerConcatenations_[i];
}
