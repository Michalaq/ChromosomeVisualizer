#include "simulationseries.h"
#include "messagehandler.h"
#include "treeitem.h"
#include <QFileInfo>

SimulationSeries::SimulationSeries(const QString& name, Session* s, int f, int l, int t) :
    SimulationItem (name, s, f, l, t)
{
    model = new ChartItem(QFileInfo(name).fileName(), this);
}

SimulationSeries::~SimulationSeries()
{
    qDeleteAll(series);
}
