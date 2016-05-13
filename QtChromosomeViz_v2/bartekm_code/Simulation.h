#ifndef PDB_PARSER_SIMULATION_H
#define PDB_PARSER_SIMULATION_H

#include "common.h"
#include <memory>
#include <cstdint>
#include <QObject>
#include "SimulationLayer.h"

class Simulation : public QObject {
    Q_OBJECT
protected:
    std::vector<std::shared_ptr<SimulationLayer>> layers_;
    frameNumber_t frameCount_;
public:
    Simulation();
    virtual ~Simulation() {};
    frameNumber_t getFrameCount() const;
    void setSimulationName(const std::string & name);
    const std::string & getSimulationName() const;
    void addSimulationLayer(std::shared_ptr<SimulationLayer> sl);
    std::shared_ptr<SimulationLayer> getSimulationLayer(int i);
    virtual std::shared_ptr<Frame> getFrame(frameNumber_t position);
signals:
    void frameCountChanged(int frameCount);
};


#endif //PDB_PARSER_SIMULATION_H
