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
    frameNumber_t frameCount_;
    frameNumber_t nextUnreadFrame_;
    TreeModel *model;
    Session *session;
public:
    Simulation(Session *s);
    virtual ~Simulation();
    frameNumber_t getFrameCount() const;
    frameNumber_t getLastFrame() const;
    void setSimulationName(const std::string & name);
    const std::string & getSimulationName() const;
    void addSimulationLayerConcatenation(std::shared_ptr<SimulationLayerConcatenation> slc, bool init = true);
    std::shared_ptr<SimulationLayerConcatenation> getSimulationLayerConcatenation(int i);
    //virtual std::shared_ptr<Frame> getFrameByNumber(frameNumber_t position);
    virtual std::shared_ptr<Frame> getFrame(frameNumber_t time);
    frameNumber_t getNextTime(frameNumber_t time) const;
    frameNumber_t getPreviousTime(frameNumber_t time) const;
    TreeModel* getModel();

    void writePOVFrame(QTextStream &stream, frameNumber_t f);
    void writePOVFrames(QTextStream &stream, frameNumber_t fbeg, frameNumber_t fend);

signals:
    void frameCountChanged(int frameCount);
};


#endif //PDB_PARSER_SIMULATION_H
