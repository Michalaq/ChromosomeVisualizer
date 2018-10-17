#include "pdbsimulationlayer.h"

PDBSimulationLayerV2::PDBSimulationLayerV2(const QString& name, int f, int l, int s) :
    first(f),
    last(l),
    stride(s)
{
    file.setFileName(name);
    file.open(QIODevice::ReadOnly);

    cacheOffsets();
}

PDBSimulationLayerV2::~PDBSimulationLayerV2()
{
    file.close();
}

#include "session.h"
#include <QTextStream>

void PDBSimulationLayerV2::loadFrame(int time, Session* session)
{
    file.seek(cache[time]);

    QString line;

    QTextStream stream(&file);

    while (file.pos() < cache[time + 1])
    {
        stream.readLineInto(&line);

        if (line.startsWith("ATOM  "))
        {
            int serial = line.midRef(6, 5).toInt() - 1;

            session->atomBuffer[serial].position.setX(line.midRef(30, 8).toFloat());
            session->atomBuffer[serial].position.setY(line.midRef(38, 8).toFloat());
            session->atomBuffer[serial].position.setZ(line.midRef(46, 8).toFloat());
        }
    }
}

void PDBSimulationLayerV2::cacheOffsets()
{
    qint64 pos = file.pos();

    QString line;

    QTextStream stream(&file);

    int i = 0;
    while (!file.atEnd() && i < 1000)
    {
        stream.readLineInto(&line);

        if (line.startsWith("HEADER"))
        {
            cache.push_back(pos);
            i++;
        }

        pos = file.pos();
    }
}
