#ifndef PDBSIMULATIONLAYSER_H
#define PDBSIMULATIONLAYSER_H


#include "simulationlayer.h"

#include <QString>
#include <QFile>
#include <QVector>

class Session;

class PDBSimulationLayerV2 : public SimulationLayerV2
{
public:
    PDBSimulationLayerV2(const QString& name, int f = 0, int l = -1, int s = 1);
    ~PDBSimulationLayerV2();

    void loadFrame(int time, Session* session);

private:
    int first, last, stride;

    QFile file;

    QVector<qint64> cache;
    void cacheOffsets();
};


#endif // PDBSIMULATIONLAYSER_H
