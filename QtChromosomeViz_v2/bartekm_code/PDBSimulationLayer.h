//
// Created by bartek on 13.12.15.
//

#ifndef PDB_PARSER_PDBSIMULATION_H
#define PDB_PARSER_PDBSIMULATION_H

#include "SimulationLayer.h"
#include <fstream>
#include <functional>
#include <map>
#include <QMutex>

using framePositionInfo_t = std::pair<frameNumber_t, frameNumber_t>;

class PDBSimulationLayer : public SimulationLayer {
private:
    QMutex accessMutex_;

	std::string fileName_;
    std::ifstream file_;

    std::map<frameNumber_t, uint64_t> cachedFramePositions_;
    std::vector<frameNumber_t> numbersAndPositions_;
	std::shared_ptr<Frame> cachedFrame_;
    frameNumber_t positionCachedFor_;
    int connectionCount_;
    bool reachedEndOfFile_;

	static Atom getAtomFromString(const std::string & str);
    std::shared_ptr<Frame> readCurrentFrame(frameNumber_t number);

    // Reads the frame header. Returns the frame number.
    frameNumber_t readFrameHeader();
    std::shared_ptr<Frame> readFrameContents(framePositionInfo_t position);
    void skipFrameContents();
public:
    PDBSimulationLayer(const std::string & name, const std::string & fileName);
    PDBSimulationLayer(const std::string & fileName);
    virtual std::shared_ptr<Frame> getFrameById(frameNumber_t position) override;
    virtual std::shared_ptr<Frame> getFrame(frameNumber_t time) override;
    virtual frameNumber_t getNextTime(frameNumber_t time) override;
    virtual frameNumber_t getPreviousTime(frameNumber_t time) override;
    virtual bool reachedEndOfFile() const override final;
};


#endif //PDB_PARSER_PDBSIMULATION_H
