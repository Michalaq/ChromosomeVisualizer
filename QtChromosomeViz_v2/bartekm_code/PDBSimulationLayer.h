//
// Created by bartek on 13.12.15.
//

#ifndef PDB_PARSER_PDBSIMULATION_H
#define PDB_PARSER_PDBSIMULATION_H

#include "SimulationLayer.h"
#include <fstream>
#include <map>

class PDBSimulationLayer : public SimulationLayer {
private:
	std::string fileName_;
    std::ifstream file_;

	std::map<frameNumber_t, uint64_t> cachedFramePositions_;
	std::shared_ptr<Frame> cachedFrame_;
    int connectionCount_;

	static Atom getAtomFromString(const std::string & str);
    std::shared_ptr<Frame> readCurrentFrame();
public:
    PDBSimulationLayer(const std::string & name, const std::string & fileName);
    PDBSimulationLayer(const std::string & fileName);
	virtual std::shared_ptr<Frame> getFrame(frameNumber_t position) override;
};


#endif //PDB_PARSER_PDBSIMULATION_H
