#include "NullSimulationLayer.h"

NullSimulationLayer::NullSimulationLayer()
    : SimulationLayer("No simulation.")
{

}

NullSimulationLayer::~NullSimulationLayer()
{

}

std::shared_ptr<Frame> NullSimulationLayer::getFrameById(frameNumber_t position)
{
    Frame f = {
        0,
        0,
        std::vector<Atom>(),
        std::map<std::string, float>()
    };
    return std::make_shared<Frame>(f);
}

std::shared_ptr<Frame> NullSimulationLayer::getFrame(frameNumber_t position)
{
    return getFrameById(position);
}

bool NullSimulationLayer::reachedEndOfFile() const
{
    return true;
}

frameNumber_t NullSimulationLayer::getNextTime(frameNumber_t time)
{
    return time;
}

frameNumber_t NullSimulationLayer::getPreviousTime(frameNumber_t time)
{
    return time;
}
