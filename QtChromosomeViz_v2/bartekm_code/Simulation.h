#ifndef PDB_PARSER_SIMULATION_H
#define PDB_PARSER_SIMULATION_H

#include "common.h"
#include <memory>
#include <QObject>
#include "SimulationLayer.h"
#include "treemodel.h"
#include "SimulationLayerConcatenation.h"

class Session;

class Simulation : public QObject {
    Q_OBJECT
protected:
    QVector<std::shared_ptr<SimulationLayerConcatenation>> layerConcatenations_;
    frameNumber_t nextUnreadFrame_;
    TreeModel *model;
public:
    Simulation(Session *s);
    virtual ~Simulation();
    frameNumber_t getFrameCount() const;
    void addSimulationLayerConcatenation(std::shared_ptr<SimulationLayerConcatenation> slc);
    void removeSimulationLayerConcatenation(std::shared_ptr<SimulationLayerConcatenation> slc);
    virtual std::shared_ptr<Frame> getFrame(frameNumber_t time, bool* ok = nullptr);
    frameNumber_t getNextTime(frameNumber_t time) const;
    frameNumber_t getPreviousTime(frameNumber_t time) const;
    TreeModel* getModel();

    void writePOVFrames(QTextStream &stream, frameNumber_t fbeg, frameNumber_t fend);

signals:
    void frameCountChanged(int frameCount);
};


#endif //PDB_PARSER_SIMULATION_H
