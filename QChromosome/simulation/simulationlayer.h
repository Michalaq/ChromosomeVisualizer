#ifndef SIMULATIONLAYER_H
#define SIMULATIONLAYER_H


#include <QJsonObject>
#include <QFile>

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

protected:
    Session* session;
    int first, last, stride;

    QFile file;

    TreeItem* model;
};

#endif // SIMULATIONLAYER_H
