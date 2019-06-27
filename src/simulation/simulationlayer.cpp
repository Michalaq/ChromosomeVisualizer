#include "simulationlayer.h"
#include "messagehandler.h"
#include "treeitem.h"
#include <QFileInfo>

SimulationLayer::SimulationLayer(const QString& name, Session* s, int f, int l, int t) : SimulationItem(name, s, f, l, t)
{
    model = new LayerItem(QFileInfo(name).fileName(), this);
}

SimulationLayer::~SimulationLayer()
{

}

TreeItem* SimulationLayer::getModel() const
{
    return model;
}

#include "pdbsimulationlayer.h"

SimulationLayer* SimulationLayer::read(const QJsonObject& json, Session* session)
{
    QString name = json["File name"].toString();
    int first = json["First"].toInt();
    int last = json["Last"].toInt();
    int stride = json["Stride"].toInt();

    QFileInfo info(name);

    if (info.completeSuffix() == "pdb" || info.completeSuffix() == "pdb.gz")
        return new PDBSimulationLayer(name, session, first, last, stride);

    throw MessageLog({QtCriticalMsg, "File format not recognized.", name, nullptr, -1, -1});
}
