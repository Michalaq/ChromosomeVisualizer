#ifndef SIMULATIONAGGREGATION_H
#define SIMULATIONAGGREGATION_H

#include "common.h"
#include <memory>
#include <cstdint>
#include <QObject>

class SimulationLayer : public QObject {
    Q_OBJECT
protected:
    std::string name_;
    frameNumber_t frameCount_;
    int connectionCount_;
    int layerId_;
    int first, last, stride;
public:
    SimulationLayer(const std::string & name);
    virtual ~SimulationLayer() {};
    frameNumber_t getFrameCount() const;
    void setSimulationLayerName(const std::string & name);
    const std::string & getSimulationLayerName() const;
    const int getConnectionCount() const;
    void setLayerId(int layerId);
    virtual std::shared_ptr<Frame> getFrameById(frameNumber_t position) = 0;
    virtual std::shared_ptr<Frame> getFrame(frameNumber_t time) = 0;
    virtual frameNumber_t getNextTime(frameNumber_t time) = 0;
    virtual frameNumber_t getPreviousTime(frameNumber_t time) = 0;
    virtual bool reachedEndOfFile() const = 0;
signals:
    void frameCountChanged(int frameCount);
};

#endif // SIMULATIONAGGREGATION_H
