#include "NullSimulation.h"

NullSimulation::NullSimulation()
    : Simulation("No simulation.")
{

}

std::shared_ptr<Frame> NullSimulation::getFrame(frameNumber_t position) {
    Frame f = {
        0,
        0,
        std::vector<Atom>(),
        std::map<std::string, float>()
    };
    return std::make_shared<Frame>(f);
}
