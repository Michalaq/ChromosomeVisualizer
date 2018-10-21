#include "simulationlayer.h"
#include "treeitem.h"
#include <QFileInfo>

SimulationLayer::SimulationLayer(const QString& name, Session* s, int f, int l, int t) :
    session(s),
    first(f),
    last(l),
    stride(t),
    model(new LayerItem(QFileInfo(name).fileName(), this, s))
{
    file.setFileName(name);
    file.open(QIODevice::ReadOnly);
}

SimulationLayer::~SimulationLayer()
{

}

TreeItem* SimulationLayer::getModel() const
{
    return model;
}

#include "binsimulationlayer.h"
#include "pdbsimulationlayer.h"

SimulationLayer* SimulationLayer::read(const QJsonObject& json, Session* session)
{
    QString name = json["File name"].toString();
    int first = json["First"].toInt();
    int last = json["Last"].toInt();
    int stride = json["Stride"].toInt();

    if (QFileInfo(name).suffix() == "bin")
        return new BINSimulationLayer(name, session, first, last, stride);

    if (QFileInfo(name).suffix() == "pdb")
        return new PDBSimulationLayer(name, session, first, last, stride);
}

void SimulationLayer::write(QJsonObject& json) const
{
    json["File name"] = file.fileName();
    json["First"] = first;
    json["Last"] = last;
    json["Stride"] = stride;
}
