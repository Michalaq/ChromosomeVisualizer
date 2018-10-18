#include "pdbsimulationlayer.h"

PDBSimulationLayerV2::PDBSimulationLayerV2(const QString& name, int f, int l, int s, bool b) :
    first(f),
    last(l),
    stride(s)
{
    buffer.resize(80 + 1);

    file.setFileName(name);
    file.open(QIODevice::ReadOnly);

    cacheHeaders(b ? 0 : INT_MAX);
}

PDBSimulationLayerV2::~PDBSimulationLayerV2()
{

}

#include "session.h"

void PDBSimulationLayerV2::loadEntry(int time, Session* session)
{
    auto entry = time <= cacheHeaders(time) ? cache[time] : cache.last();

    file.seek(entry.first);

    while (file.pos() < entry.second)
    {
        file.readLine(buffer.data(), buffer.size());

        if (buffer.startsWith("ATOM  "))
        {
            int serial = buffer.mid(6, 5).trimmed().toInt() - 1;

            session->atomBuffer[serial].position.setX(buffer.mid(30, 8).trimmed().toFloat());
            session->atomBuffer[serial].position.setY(buffer.mid(38, 8).trimmed().toFloat());
            session->atomBuffer[serial].position.setZ(buffer.mid(46, 8).trimmed().toFloat());
        }
    }
}

qint64 PDBSimulationLayerV2::skipHeader()
{
    while (true)
    {
        qint64 pos = file.pos();

        if (file.readLine(buffer.data(), buffer.size()) == -1)
        {
            atEnd = true;
            return pos;
        }

        if (buffer.startsWith("HEADER"))
        {
            atEnd = false;
            return pos;
        }
    }
}

int PDBSimulationLayerV2::cacheHeaders(int limit)
{
    if (atEnd)
        return j - 1;

    file.seek(pos);

    while (i <= first)
    {
        range.first = skipHeader();
        i++;

        if (atEnd)
            return j - 1;
    }

    while (j <= limit)
    {
        range.second = skipHeader();
        i++;

        cache.append(range);
        j++;

        range.first = range.second;

        for (int k = 1; k < stride; k++)
        {
            range.first = skipHeader();
            i++;

            if (i > last)
                atEnd = true;

            if (atEnd)
                return j - 1;
        }
    }

    pos = file.pos();

    return j - 1;
}
