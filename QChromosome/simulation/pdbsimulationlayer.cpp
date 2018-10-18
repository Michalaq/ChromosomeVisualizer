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
    cacheHeaders(time);

    auto entry = time < cache.size() ? cache[time] : cache.last();

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

int PDBSimulationLayerV2::lastEntry() const
{
    return cache.size() - 1;
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

void PDBSimulationLayerV2::cacheHeaders(int limit)
{
    if (atEnd)
        return;

    file.seek(pos);

    while (i <= first)
    {
        range.first = skipHeader();
        i++;

        if (atEnd)
            return;
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
                return;
        }
    }

    pos = file.pos();
}
