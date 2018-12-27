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
