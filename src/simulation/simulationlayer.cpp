#include "simulationlayer.h"
#include "messagehandler.h"
#include "treeitem.h"
#include <QFileInfo>

SimulationLayer::SimulationLayer(const QString& name, Session* s, int f, int l, int t) :
    SimulationItem(name, s, f, l, t)
{
    model = new LayerItem(QFileInfo(name).fileName(), this);
}

SimulationLayer::~SimulationLayer()
{

}
