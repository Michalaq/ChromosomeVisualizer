#include "pdbsimulationlayer.h"

PDBSimulationLayer::PDBSimulationLayer(const QString& name, Session* s, int f, int l, int t, bool b) :
    SimulationLayer(name, s, f, l, t)
{
    buffer.resize(128);

    if (cacheHeaders(b ? 0 : INT_MAX) == 0)
        makeModel();
}

PDBSimulationLayer::~PDBSimulationLayer()
{
    qDeleteAll(functions);
}

#include "session.h"

void PDBSimulationLayer::readEntry(int time, char* data, std::size_t stride, std::size_t pointer)
{
    auto entry = time <= cacheHeaders(time) ? cache[time] : cache.last();

    file->seek(entry.first);

    while (file->pos() < entry.second)
    {
        file->readLine(buffer.data(), buffer.size());

        if (buffer.startsWith("ATOM  "))
        {
            int serial = buffer.mid(6, 5).trimmed().toUInt() - 1;

            float* position = reinterpret_cast<float*>(data + stride * (model->atomOffset() + serial) + pointer);

            position[0] = buffer.mid(30, 8).trimmed().toFloat();
            position[1] = buffer.mid(38, 8).trimmed().toFloat();
            position[2] = buffer.mid(46, 8).trimmed().toFloat();
        }
    }
}

qint64 PDBSimulationLayer::skipHeader()
{
    while (true)
    {
        qint64 pos = file->pos();

        if (!file->readLine(buffer.data(), buffer.size()))
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

int PDBSimulationLayer::cacheHeaders(int time)
{
    if (atEnd)
        return j - 1;

    file->seek(pos);

    while (i <= first)
    {
        range.first = skipHeader();
        i++;

        if (atEnd)
            return j - 1;
    }

    while (j <= time)
    {
        readTitle();

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

    pos = file->pos();

    return j - 1;
}

int PDBSimulationLayer::lastEntry() const
{
    return j - 1;
}

#include "treeitem.h"
#include <QBitArray>

void PDBSimulationLayer::makeModel()
{
    QVector<QByteArray> atoms;
    QVector<QPair<int, int>> chains;

    QPair<int, int> range(-1, -1);

    file->seek(cache[0].first);

    while (file->pos() < cache[0].second)
    {
        file->readLine(buffer.data(), buffer.size());

        if (buffer.startsWith("ATOM  "))
            atoms.append(buffer.mid(17, 3).trimmed());

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

    int a_offset = session->atomBuffer.size();
    int c_offset = session->chainBuffer[0].size();

    session->atomBuffer.resize(a_offset + atoms.size());
    session->chainBuffer[0].resize(c_offset + chains.size());
    session->chainBuffer[1].resize(c_offset + chains.size());

    for (int i = 0; i < chains.size(); i++)
    {
        used.fill(true, chains[i].first, ++chains[i].second);

        auto chain = new ChainItem(i, c_offset, session, model);

        QMap<QByteArray, ResidueItem*> residues;

        for (int j = chains[i].first; j < chains[i].second; j++)
        {
            auto& residue = atoms[j];

            if (!residues.contains(residue))
                residues[residue] = new ResidueItem(residue, chain);

            new AtomItem(j, a_offset, session, residues[residue]);
        }
    }

    QMap<QByteArray, ResidueItem*> residues;

    for (int j = 0; j < atoms.size(); j++)
    {
        if (used.testBit(j))
            continue;

        auto& residue = atoms[j];

        if (!residues.contains(residue))
            residues[residue] = new ResidueItem(residue, model);

        new AtomItem(j, a_offset, session, residues[residue]);
    }

    for (int j = 0; j < chains.size(); j++)
    {
        session->chainBuffer[0][c_offset + j] = a_offset + chains[j].first;
        session->chainBuffer[1][c_offset + j] = chains[j].second - chains[j].first;
    }
}

void PDBSimulationLayer::readTitle()
{
    static const QRegularExpression re(";([^=]*)=([^;]*)");

    file->readLine(buffer.data(), buffer.size());

    if (buffer.startsWith("TITLE "))
    {
        auto i = re.globalMatch(buffer);

        while (i.hasNext())
        {
            auto match = i.next();

            auto name = match.capturedRef(1).trimmed().toString();
            auto value = match.capturedRef(2).trimmed().toDouble();

            if (!functions.contains(name))
            {
                auto series = new QtCharts::QLineSeries;
                series->setName(name);

                session->chart->addSeries(series);
                functions.insert(name, series);
            }

            functions[name]->append(j, value);
        }
    }
}
