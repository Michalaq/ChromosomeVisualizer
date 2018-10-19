#include "simulationlayer.h"

SimulationLayerV2::~SimulationLayerV2()
{

}

#include <QFileInfo>
#include "pdbsimulationlayer.h"

SimulationLayerV2* SimulationLayerV2::read(const QJsonObject& json, Session* session)
{
    QString name = json["File name"].toString();

    if (QFileInfo(name).suffix() == "pdb")
    {
        int first = json["First"].toInt();
        int last = json["Last"].toInt();
        int stride = json["Stride"].toInt();

        return new PDBSimulationLayerV2(name, session, first, last, stride);
    }

    Q_ASSERT(false);
}
