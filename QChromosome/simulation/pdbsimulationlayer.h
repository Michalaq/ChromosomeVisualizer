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
    PDBSimulationLayerV2(const QString& name, int f = 0, int l = INT_MAX, int s = 1, bool b = true);
    ~PDBSimulationLayerV2() override;

    void loadEntry(int time, Session* session) override;

    int cacheHeaders(int limit) override;

private:
    int first, last, stride;

    QFile file;
    QByteArray buffer;

    QVector<QPair<qint64, qint64>> cache;

    qint64 pos = 0;
    int i = 0, j = 0;
    QPair<qint64, qint64> range;

    bool atEnd = false;

    qint64 skipHeader();
};


#endif // PDBSIMULATIONLAYSER_H
