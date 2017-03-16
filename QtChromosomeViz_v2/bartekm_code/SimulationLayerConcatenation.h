#ifndef SIMULATIONLAYERCONCATENATION_H
#define SIMULATIONLAYERCONCATENATION_H

#include "common.h"
#include "SimulationLayer.h"
#include "TimeTransformation.h"
#include <memory>
#include <cstdint>
#include <vector>
#include <QObject>

class SimulationLayerConcatenation : public QObject {
    Q_OBJECT
private:
    std::vector<std::shared_ptr<SimulationLayer>> layers_;
    std::vector<frameNumber_t> aggregatedFrameCounts_;
    TimeTransformation transform_;
    frameNumber_t frameCount_;
    frameNumber_t getLayerBaseFrameNumber(int layer);
    int layerId_;
public:
    SimulationLayerConcatenation();
    SimulationLayerConcatenation(std::shared_ptr<SimulationLayer> sl);
    ~SimulationLayerConcatenation();
    frameNumber_t getFrameCount() const;
    void appendSimulationLayer(std::shared_ptr<SimulationLayer> sl);
    int getConnectionCount() const;

    TimeTransformation & getTransform();
    void setLayerId(int layerId);

    // Returns a frame in absolute time.
    std::shared_ptr<Frame> getFrame(frameNumber_t position);

    // Gets next time of an interesting frame after 'time',
    // Returns 'time' if there are no more interesting frames.
    frameNumber_t getNextTime(frameNumber_t time);

    // Gets previous time of an interesting frame after 'time'.
    // Returns 'time' if 'time' <= 0.
    frameNumber_t getPreviousTime(frameNumber_t time);

    void read(const QJsonArray& json);
    void write(QJsonArray& json) const;

signals:
    void frameCountChanged(int frameCount);
};

#endif // SIMULATIONLAYERCONCATENATION_H
