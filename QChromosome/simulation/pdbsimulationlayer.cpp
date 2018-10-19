#include "pdbsimulationlayer.h"

PDBSimulationLayerV2::PDBSimulationLayerV2(const QString& name, Session* s, int f, int l, int t, bool b) :
    session(s),
    first(f),
    last(l),
    stride(t)
{
    buffer.resize(80 + 1);

    file.setFileName(name);
    file.open(QIODevice::ReadOnly);

    if (cacheHeaders(b ? 0 : INT_MAX) == 0)
        makeModel();
}

PDBSimulationLayerV2::~PDBSimulationLayerV2()
{

}

#include "session.h"

void PDBSimulationLayerV2::loadEntry(int time)
{
    auto entry = time <= cacheHeaders(time) ? cache[time] : cache.last();

    file.seek(entry.first);

    while (file.pos() < entry.second)
    {
        file.readLine(buffer.data(), buffer.size());

        if (buffer.startsWith("ATOM  "))
        {
            int serial = buffer.mid(6, 5).trimmed().toUInt() - 1;

            session->atomBuffer[a_offset + serial].position.setX(buffer.mid(30, 8).trimmed().toFloat());
            session->atomBuffer[a_offset + serial].position.setY(buffer.mid(38, 8).trimmed().toFloat());
            session->atomBuffer[a_offset + serial].position.setZ(buffer.mid(46, 8).trimmed().toFloat());
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

int PDBSimulationLayerV2::cacheHeaders(int time)
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

    while (j <= time)
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

TreeItem* PDBSimulationLayerV2::getModel() const
{
    return model;
}

#include "preferences.h"
#include "treeitem.h"
#include <QFileInfo>
#include <QBitArray>

void PDBSimulationLayerV2::makeModel()
{
    QVector<uint> atoms;
    QVector<QPair<int, int>> chains;

    QPair<int, int> range(-1, -1);

    file.seek(cache[0].first);

    while (file.pos() < cache[0].second)
    {
        file.readLine(buffer.data(), buffer.size());

        if (buffer.startsWith("ATOM  "))
        {
            uint residue = Preferences::getInstance()->rs2typename(buffer.mid(17, 3).trimmed());

            atoms.append(residue);
        }

        if (buffer.startsWith("CONECT"))
        {
            int serial = buffer.mid(6, 5).trimmed().toUInt() - 1;
            int bonded = buffer.mid(11, 5).trimmed().toUInt() - 1;

            if (range.second == serial)
                range.second = bonded;
            else
            {
                chains.append(range);
                range = {serial, bonded};
            }
        }
    }

    chains.append(range);
    chains.pop_front();

    QBitArray used(atoms.size(), false);

    a_offset = session->atomBuffer.size();
    int c_offset = session->chainBuffer[0].size();

    session->atomBuffer.resize(a_offset + atoms.size());
    session->chainBuffer[0].resize(c_offset + chains.size());
    session->chainBuffer[1].resize(c_offset + chains.size());

    model = new LayerItem(QFileInfo(file).fileName(), this, session);

    for (int i = 0; i < chains.size(); i++)
    {
        used.fill(true, chains[i].first, ++chains[i].second);

        auto chain = new ChainItem(i, c_offset, session, model);

        QMap<uint, ResidueItem*> residues;

        for (int j = chains[i].first; j < chains[i].second; j++)
        {
            uint residue = atoms[j];

            if (!residues.contains(residue))
                residues[residue] = new ResidueItem(residue, chain);

            new AtomItem(j, a_offset, session, residues[residue]);
        }
    }

    QMap<uint, ResidueItem*> residues;

    for (int j = 0; j < atoms.size(); j++)
    {
        if (used.testBit(j))
            continue;

        uint residue = atoms[j];

        if (!residues.contains(residue))
            residues[residue] = new ResidueItem(residue, model);

        new AtomItem(j, a_offset, session, residues[residue]);
    }

    for (int j = 0; j < chains.size(); j++)
    {
        session->chainBuffer[0][c_offset + j] = chains[j].first;
        session->chainBuffer[1][c_offset + j] = chains[j].second - chains[j].first;
    }
}

void PDBSimulationLayerV2::write(QJsonObject& json) const
{
    json["File name"] = file.fileName();
    json["First"] = first;
    json["Last"] = last;
    json["Stride"] = stride;
}
