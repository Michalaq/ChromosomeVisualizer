#include "Simulation.h"


Simulation::Simulation()
    : frameCount_(0),
      model(nullptr)
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
    int i = 0;
    for (const auto & l : layerConcatenations_) {
        std::shared_ptr<Frame> f2 = l->getFrame(position);
        f.no = f2->no;
        f.step = f2->step;

        for (const auto& p : f2->functionValues) {
            f.functionValues[p.first + "_layer_" + std::to_string(i)] = p.second;
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
//    for (const auto& p : f.connectedRanges) {
//        std::cout << "AGGREGATED CONNECTION: " << p.first << ", " << p.second << std::endl;
//    }
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
    model = new TreeModel(getFrame(0)->atoms, this);
}

std::shared_ptr<SimulationLayerConcatenation> Simulation::getSimulationLayerConcatenation(int i)
{
    return layerConcatenations_[i];
}

TreeModel* Simulation::getModel()
{
    return model;
}
