//
// Created by bartek on 09.12.15.
//

#ifndef PDB_PARSER_SIMULATION_H
#define PDB_PARSER_SIMULATION_H

#include "common.h"
#include <memory>
#include <cstdint>

using frameNumber_t = uint64_t;

class Simulation {
public:
	virtual ~Simulation() {};
	virtual std::shared_ptr<Frame> getFrame(frameNumber_t position) = 0;
};


#endif //PDB_PARSER_SIMULATION_H
