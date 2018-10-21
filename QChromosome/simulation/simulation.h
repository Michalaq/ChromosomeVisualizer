#ifndef SIMULATION_H
#define SIMULATION_H


#include "simulationlayer.h"
#include <QVector>
#include <QTextStream>

class Session;
class TreeModel;

class Simulation : public QVector<SimulationLayer*>
{
public:
    Simulation(Session* s);
    ~Simulation();

    void readEntry(int time, char* data, std::size_t stride, std::size_t pointer);

    int cacheHeaders(int time);

    TreeModel* getModel() const;

    void prepend(SimulationLayer* value);

    void writePOVFrames(QTextStream& stream, int fbeg, int fend);

private:
    TreeModel* model;
};

#endif // SIMULATION_H
