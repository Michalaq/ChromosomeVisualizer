#ifndef PDB_PARSER_SIMULATION_H
#define PDB_PARSER_SIMULATION_H

#include "common.h"
#include <memory>
#include <cstdint>


class Simulation {
protected:
    std::string name_;
    frameNumber_t frameCount_;
public:
    Simulation(const std::string & name);
    virtual ~Simulation() {};
    frameNumber_t getFrameCount() const;
    void setSimulationName(const std::string & name);
    const std::string & getSimulationName() const;
	virtual std::shared_ptr<Frame> getFrame(frameNumber_t position) = 0;
};


#endif //PDB_PARSER_SIMULATION_H
