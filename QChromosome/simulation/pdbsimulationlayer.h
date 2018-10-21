#ifndef PDBSIMULATIONLAYSER_H
#define PDBSIMULATIONLAYSER_H


#include "simulationlayer.h"
#include <QVector>

class Session;
class LayerItem;

class PDBSimulationLayer : public SimulationLayer
{
public:
    PDBSimulationLayer(const QString& name, Session* s, int f = 0, int l = INT_MAX, int t = 1, bool b = true);
    ~PDBSimulationLayer() override;

    void readEntry(int time, char* data, std::size_t stride, std::size_t pointer) override;

    int cacheHeaders(int time) override;

private:
    QByteArray buffer;

    QVector<QPair<qint64, qint64>> cache;

    qint64 pos = 0;
    int i = 0, j = 0;
    QPair<qint64, qint64> range;

    bool atEnd = false;

    qint64 skipHeader();

    void makeModel();

    int a_offset;
};


#endif // PDBSIMULATIONLAYSER_H
