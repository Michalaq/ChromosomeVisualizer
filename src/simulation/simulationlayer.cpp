#include "simulationlayer.h"
#include "treeitem.h"
#include <QFileInfo>

SimulationLayer::SimulationLayer(const QString& name, Session* s, int f, int l, int t) :
    session(s),
    first(f),
    last(l),
    stride(t)
{
    QFileInfo info(name);

    if (info.suffix() == "gz")
        file = new GzFile;
    else
        file = new File;

    file->setFileName(name);
    file->open(QIODevice::ReadOnly);

    model = new LayerItem(info.fileName(), this, session);
}

SimulationLayer::~SimulationLayer()
{
    delete file;
}

TreeItem* SimulationLayer::getModel() const
{
    return model;
}

#include "pdbsimulationlayer.h"
#include "messagehandler.h"

SimulationLayer* SimulationLayer::read(const QJsonObject& json, Session* session)
{
    QString name = json["File name"].toString();
    int first = json["First"].toInt();
    int last = json["Last"].toInt();
    int stride = json["Stride"].toInt();

    QFileInfo info(name);

    if (info.completeSuffix() == "pdb" || info.completeSuffix() == "pdb.gz")
        return new PDBSimulationLayer(name, session, first, last, stride);

    qcCritical("File format not recognized.", name, -1, -1);
    return nullptr;
}

void SimulationLayer::write(QJsonObject& json) const
{
    json["File name"] = file->fileName();
    json["First"] = first;
    json["Last"] = last;
    json["Stride"] = stride;
}
