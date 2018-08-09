#ifndef PDB_PARSER_SIMULATION_H
#define PDB_PARSER_SIMULATION_H

#include "common.h"
#include <memory>
#include <cstdint>
#include <QObject>
#include "SimulationLayer.h"
#include "treemodel.h"
#include "SimulationLayerConcatenation.h"

class Session;

class Simulation : public QObject {
    Q_OBJECT
protected:
    std::vector<std::shared_ptr<SimulationLayerConcatenation>> layerConcatenations_;
    frameNumber_t nextUnreadFrame_;
    TreeModel *model;
    Session *session;
public:
    Simulation(Session *s);
    virtual ~Simulation();
    frameNumber_t getFrameCount() const;
    void addSimulationLayerConcatenation(std::shared_ptr<SimulationLayerConcatenation> slc, bool init = true);
    virtual std::shared_ptr<Frame> getFrame(frameNumber_t time, bool* expanded = nullptr);
    frameNumber_t getNextTime(frameNumber_t time) const;
    frameNumber_t getPreviousTime(frameNumber_t time) const;
    TreeModel* getModel();

    void writePOVFrame(QTextStream &stream, frameNumber_t f);
    void writePOVFrames(QTextStream &stream, frameNumber_t fbeg, frameNumber_t fend);

signals:
    void frameCountChanged(int frameCount);
};


#endif //PDB_PARSER_SIMULATION_H
