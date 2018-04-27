#ifndef SIMULATIONAGGREGATION_H
#define SIMULATIONAGGREGATION_H

#include "common.h"
#include <memory>
#include <cstdint>
#include <QObject>

class UntransformedSimulationLayer : public QObject {
    Q_OBJECT
protected:
    std::string name_;
    frameNumber_t frameCount_;
    int connectionCount_;
    int layerId_;
public:
    UntransformedSimulationLayer(const std::string & name);
    virtual ~UntransformedSimulationLayer() {};
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

class SimulationLayer : public QObject {
    Q_OBJECT
protected:
    std::shared_ptr<UntransformedSimulationLayer> layer;
    frameNumber_t first, last, stride;
public:
    SimulationLayer(std::shared_ptr<UntransformedSimulationLayer> l);
    virtual ~SimulationLayer() {};
    frameNumber_t getFrameCount() const;
    void setSimulationLayerName(const std::string & name);
    const std::string & getSimulationLayerName() const;
    const int getConnectionCount() const;
    void setLayerId(int layerId);
    std::shared_ptr<Frame> getFrame(frameNumber_t time);
    frameNumber_t getNextTime(frameNumber_t time);
    frameNumber_t getPreviousTime(frameNumber_t time);
    bool reachedEndOfFile() const;

    static std::shared_ptr<SimulationLayer> read(const QJsonObject& json);
    void write(QJsonObject& json) const;

signals:
    void frameCountChanged(int frameCount);
    friend class ImportDialog;
};

#endif // SIMULATIONAGGREGATION_H
