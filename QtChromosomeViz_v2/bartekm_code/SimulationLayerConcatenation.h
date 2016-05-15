#ifndef SIMULATIONLAYERCONCATENATION_H
#define SIMULATIONLAYERCONCATENATION_H

#include "common.h"
#include "SimulationLayer.h"
#include <memory>
#include <cstdint>
#include <vector>
#include <QObject>

class SimulationLayerConcatenation : public QObject {
    Q_OBJECT
private:
    std::vector<std::shared_ptr<SimulationLayer>> layers_;
    std::vector<frameNumber_t> aggregatedFrameCounts_;
    frameNumber_t frameCount_;
    frameNumber_t getLayerBaseFrameNumber(int layer);
public:
    SimulationLayerConcatenation();
    SimulationLayerConcatenation(std::shared_ptr<SimulationLayer> sl);
    virtual ~SimulationLayerConcatenation();
    frameNumber_t getFrameCount() const;
    void appendSimulationLayer(std::shared_ptr<SimulationLayer> sl);
    int getConnectionCount() const;
    std::shared_ptr<Frame> getFrame(frameNumber_t position);
signals:
    void frameCountChanged(int frameCount);
};

#endif // SIMULATIONLAYERCONCATENATION_H
