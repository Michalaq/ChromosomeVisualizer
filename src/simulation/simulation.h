#ifndef SIMULATION_H
#define SIMULATION_H


#include <QVector>

class Session;
class TreeModel;
class SimulationItem;

class Simulation
{
public:
    Simulation(Session* s);
    ~Simulation();

    void readEntry(int time, char* data, std::size_t stride, std::size_t pointer);

    int cacheHeaders(int time);

    TreeModel* getModel() const;

    void prepend(SimulationItem* value);
    void removeOne(SimulationItem* layer);

    int lastEntry() const;

private:
    QVector<SimulationItem*> items;

    TreeModel* model;
    Session* session;
};

#endif // SIMULATION_H
