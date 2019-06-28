#ifndef SIMULATIONITEM_H
#define SIMULATIONITEM_H


#include <QJsonObject>
#include "iodevice.h"

class Session;
class TreeItem;

class SimulationItem
{
public:
    SimulationItem(const QString& name, Session* s, int f = 0, int l = INT_MAX, int t = 1);
    virtual ~SimulationItem();

    virtual void readEntry(int time, char* data, std::size_t stride, std::size_t pointer) = 0;

    virtual int cacheHeaders(int time) = 0;

    TreeItem* getModel() const;

    static SimulationItem* read(const QJsonObject& json, Session* session);
    void write(QJsonObject& json) const;

    virtual int lastEntry() const = 0;

protected:
    Session* session;
    int first, last, stride;

    IODevice* file;

    TreeItem* model;
};

#endif // SIMULATIONITEM_H
