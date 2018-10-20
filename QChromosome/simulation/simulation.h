#ifndef SIMULATION_H
#define SIMULATION_H


#include "simulationlayer.h"
#include <QVector>
#include <QTextStream>

class Session;
class TreeModel;

class SimulationV2 : public QVector<SimulationLayerV2*>
{
public:
    SimulationV2(Session* s);
    ~SimulationV2();

    void readEntry(int time, char* data, std::size_t stride, std::size_t pointer);

    int cacheHeaders(int time);

    TreeModel* getModel() const;

    void prepend(SimulationLayerV2* value);

    void writePOVFrames(QTextStream& stream, int fbeg, int fend);

private:
    TreeModel* model;
};

#endif // SIMULATION_H
