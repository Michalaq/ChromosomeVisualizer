#include "simulationseries.h"
#include "messagehandler.h"
#include <QFileInfo>

SimulationSeries::SimulationSeries(const QString& name, Session* s, int f, int l, int t) : SimulationItem (name, s, f, l, t)
{

}

SimulationSeries::~SimulationSeries()
{
    qDeleteAll(series);
}

#include "xvgsimulationseries.h"

SimulationSeries* SimulationSeries::read(const QJsonObject& json, Session* session)
{
    QString name = json["File name"].toString();
    int first = json["First"].toInt();
    int last = json["Last"].toInt();
    int stride = json["Stride"].toInt();

    QFileInfo info(name);

    if (info.completeSuffix() == "xvg" || info.completeSuffix() == "xvg.gz")
        return new XVGSimulationSeries(name, session, first, last, stride);

    throw MessageLog({QtCriticalMsg, "File format not recognized.", name, nullptr, -1, -1});
}
