#ifndef PDBSIMULATIONLAYSER_H
#define PDBSIMULATIONLAYSER_H


#include "simulationlayer.h"
#include <QVector>

struct CacheLog
{
    qint64 first;
    qint64 last;
    int line;
};

class Session;
class LayerItem;

class PDBSimulationLayer : public SimulationLayer
{
public:
    PDBSimulationLayer(const QString& name, Session* s, int f = 0, int l = INT_MAX, int t = 1, bool b = true);
    ~PDBSimulationLayer() override;

    void readEntry(int time, char* data, std::size_t stride, std::size_t pointer) override;

    int cacheHeaders(int time) override;

    int lastEntry() const override;

    void remove() override;

private:
    QByteArray buffer;

    uint* offset;
    QVector<CacheLog> cache;

    qint64 pos = 0;
    int i = 0, j = -1, line = 0;
    CacheLog range;

    bool atEnd = false;

    bool skipHeader();

    void makeModel();

    QVector<QPair<uint, uint>> a_range;
    QVector<void*> c_range;
};

#endif // PDBSIMULATIONLAYSER_H
