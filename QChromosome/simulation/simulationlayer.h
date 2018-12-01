#ifndef SIMULATIONLAYER_H
#define SIMULATIONLAYER_H


#include <QJsonObject>
#include "iodevice.h"

class TreeItem;
class Session;

class SimulationLayer
{
public:
    SimulationLayer(const QString& name, Session* s, int f = 0, int l = INT_MAX, int t = 1);
    virtual ~SimulationLayer();

    virtual void readEntry(int time, char* data, std::size_t stride, std::size_t pointer) = 0;

    virtual int cacheHeaders(int time) = 0;

    virtual TreeItem* getModel() const;

    static SimulationLayer* read(const QJsonObject& json, Session* session);
    virtual void write(QJsonObject& json) const;

    virtual int lastEntry() const = 0;

    virtual std::tuple<int, int, int> remove() = 0;
    virtual void shift(std::tuple<int, int, int> offset) = 0;

protected:
    Session* session;
    int first, last, stride;

    IODevice* file;

    TreeItem* model;
};

#endif // SIMULATIONLAYER_H
