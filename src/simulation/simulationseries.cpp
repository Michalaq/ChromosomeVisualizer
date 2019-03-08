#include "simulationseries.h"
#include <QFileInfo>

SimulationSeries::SimulationSeries(const QString& name, Session* s, int f, int l, int t) :
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
}

SimulationSeries::~SimulationSeries()
{
    qDeleteAll(series);
}

#include "xvgsimulationseries.h"
#include "messagehandler.h"

SimulationSeries* SimulationSeries::read(const QJsonObject& json, Session* session)
{
    QString name = json["File name"].toString();
    int first = json["First"].toInt();
    int last = json["Last"].toInt();
    int stride = json["Stride"].toInt();

    QFileInfo info(name);

    if (info.completeSuffix() == "xvg" || info.completeSuffix() == "xvg.gz")
        return new XVGSimulationSeries(name, session, first, last, stride);

    qcCritical("File format not recognized.", name, -1, -1);
    return nullptr;
}

void SimulationSeries::write(QJsonObject& json) const
{
    json["File name"] = file->fileName();
    json["First"] = first;
    json["Last"] = last;
    json["Stride"] = stride;
}
