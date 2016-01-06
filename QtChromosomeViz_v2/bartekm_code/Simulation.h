#ifndef PDB_PARSER_SIMULATION_H
#define PDB_PARSER_SIMULATION_H

#include "common.h"
#include <memory>
#include <cstdint>
#include <QObject>

class Simulation : public QObject {
    Q_OBJECT
protected:
    std::string name_;
    frameNumber_t frameCount_;
    int connectionCount_;
public:
    Simulation(const std::string & name);
    virtual ~Simulation() {};
    frameNumber_t getFrameCount() const;
    void setSimulationName(const std::string & name);
    const std::string & getSimulationName() const;
    const int getConnectionCount() const;
	virtual std::shared_ptr<Frame> getFrame(frameNumber_t position) = 0;
signals:
    void frameCountChanged(int frameCount);
};


#endif //PDB_PARSER_SIMULATION_H
