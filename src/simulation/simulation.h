#ifndef SIMULATION_H
#define SIMULATION_H


#include "simulationlayer.h"
#include "simulationseries.h"
#include <QVector>
#include <QTextStream>

class Session;
class TreeModel;

class Simulation
{
public:
    Simulation(Session* s);
    ~Simulation();

    void readEntry(int time, char* data, std::size_t stride, std::size_t pointer);

    int cacheHeaders(int time);

    TreeModel* getModel() const;

    void prepend(SimulationLayer* value);
    void prepend(SimulationSeries* value);

    void removeOne(SimulationLayer* layer);
    void removeOne(SimulationSeries* layer);

    void read(const QJsonArray& json);
    void write(QJsonArray& json) const;

    void writePOVFrames(QTextStream& stream, int fbeg, int fend);

    int lastEntry() const;

private:
    QVector<SimulationLayer*> layers;
    QVector<SimulationSeries*> series;

    TreeModel* model;
    Session* session;
};

#endif // SIMULATION_H
