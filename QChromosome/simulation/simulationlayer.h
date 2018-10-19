#ifndef SIMULATIONLAYER_H
#define SIMULATIONLAYER_H


#include <QJsonObject>

class TreeItem;
class Session;

class SimulationLayerV2
{
public:
    virtual ~SimulationLayerV2();

    virtual void loadEntry(int time) = 0;

    virtual int cacheHeaders(int time) = 0;

    virtual TreeItem* getModel() const = 0;

    static SimulationLayerV2* read(const QJsonObject& json, Session* session);
    virtual void write(QJsonObject& json) const = 0;
};

#endif // SIMULATIONLAYER_H
