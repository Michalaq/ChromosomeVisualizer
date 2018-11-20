#include "pdbsimulationlayer.h"

static const uint SERIAL_MAX = 100000;

PDBSimulationLayer::PDBSimulationLayer(const QString& name, Session* s, int f, int l, int t, bool b) :
    SimulationLayer(name, s, f, l, t),
    offset(new uint[SERIAL_MAX])
{
    buffer.resize(128);

    if (cacheHeaders(b ? 0 : INT_MAX) == 0)
        makeModel();
}

PDBSimulationLayer::~PDBSimulationLayer()
{
    delete[] offset;
}

#include "session.h"

void PDBSimulationLayer::readEntry(int time, char* data, std::size_t stride, std::size_t pointer)
{
    auto entry = time <= cacheHeaders(time) ? cache[time] : cache.last();

    file->seek(entry.first);

    while (file->pos() < entry.second)
    {
        file->readLine(buffer.data(), buffer.size());

        if (buffer.startsWith("ATOM  ") || buffer.startsWith("HETATM"))
        {
            uint serial = buffer.mid(6, 5).trimmed().toUInt();

            float* position = reinterpret_cast<float*>(data + stride * offset[serial] + pointer);

            position[0] = buffer.mid(30, 8).trimmed().toFloat();
            position[1] = buffer.mid(38, 8).trimmed().toFloat();
            position[2] = buffer.mid(46, 8).trimmed().toFloat();
        }
    }
}

qint64 PDBSimulationLayer::skipHeader()
{
    static const QRegularExpression re(";([^=]*)=([^;]*)");

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

#include <QBitArray>

static QVector<QPair<uint, uint>>* connections;
static QVector<QPair<uint, uint>> circuit;
static QBitArray visited;

void traverse(uint v)
{
    while (!connections[v].isEmpty())
    {
        auto w = connections[v].takeLast();

        if (!visited.testBit(w.first))
        {
            visited.setBit(w.first);
            traverse(w.second);
            circuit.append(w);
        }
    }
}

#include "treeitem.h"

void PDBSimulationLayer::makeModel()
{
    bool ok;
    uint v = a_range.first = session->atomBuffer.size(), e = 0;

    connections = new QVector<QPair<uint, uint>>[SERIAL_MAX];
    QMap<QByteArray, QPair<ChainItem*, QMap<QByteArray, ResidueItem*>>> structure;

    file->seek(cache[0].first);

    while (file->pos() < cache[0].second)
    {
        file->readLine(buffer.data(), buffer.size());

        if (buffer.startsWith("ATOM  ") || buffer.startsWith("HETATM"))
        {
            uint serial = buffer.mid(6, 5).trimmed().toUInt(&ok);
            if (!ok) continue;

            auto name = buffer.mid(12, 4).trimmed();

            auto resName = buffer.mid(17, 3).trimmed();

            auto chainID = buffer.mid(21, 1).trimmed();

            if (!structure.contains(chainID))
                structure.insert(chainID, {new ChainItem(chainID, session, model), {}});

            auto& chain = structure[chainID];

            if (!chain.second.contains(resName))
                chain.second.insert(resName, new ResidueItem(resName, session, chain.first));

            new AtomItem(serial, name, offset[serial] = v++, session, chain.second[resName]);
        }

        if (buffer.startsWith("CONECT"))
        {
            uint serial = buffer.mid(6, 5).trimmed().toUInt(&ok);
            if (!ok) continue;

            for (int column : {11, 16, 21, 26})
            {
                uint bonded = buffer.mid(column, 5).trimmed().toUInt(&ok);

                if (ok && serial < bonded)
                {
                    connections[serial].append({e, bonded});
                    connections[bonded].append({e, serial});
                    e++;
                }
            }
        }
    }

    session->atomBuffer.resize(a_range.second = v);

    uint f = e;

    QVector<uint> odd;

    for (uint i = 0; i < SERIAL_MAX; i++)
        if (connections[i].count() & 1)
            odd.append(i);

    for (int i = 0; i < odd.count(); i += 2)
    {
        connections[odd[i]].append({f, odd[i + 1]});
        connections[odd[i + 1]].append({f, odd[i]});
        f++;
    }

    c_range.first = session->chainIndicesBuffer.size();

    visited.fill(false, f);

    for (uint i = 0; i < SERIAL_MAX; i++)
        if (!connections[i].isEmpty())
        {
            traverse(i);

            QVector<QVector<uint>> chains;
            QVector<uint> chain;

            for (auto i = circuit.rbegin(); i != circuit.rend(); i++)
            {
                if (i->first < e)
                    chain.append(offset[i->second]);
                else
                {
                    chains.append(chain);
                    chain = {offset[i->second]};
                }
            }

            if (chains.isEmpty())
            {
                chain.prepend(chain.last());
                chains = {chain};
            }
            else
                chains.prepend(chain + chains.takeFirst());

            for (const auto& chain : chains)
            {
                session->chainCountBuffer.append(chain.count());

                auto indices = new GLuint[chain.size()];
                std::reverse_copy(chain.begin(), chain.end(), indices);

                session->chainIndicesBuffer.append(indices);
            }

            circuit.clear();
        }

    c_range.second = session->chainIndicesBuffer.size();

    delete[] connections;
    visited.clear();
}

QPair<int, int> PDBSimulationLayer::remove()
{
    session->simulation->removeOne(this);

    session->atomBuffer.remove(a_range.first, a_range.second - a_range.first);
    session->indices.remove(a_range.first, a_range.second - a_range.first);

    session->chainCountBuffer.remove(c_range.first, c_range.second - c_range.first);
    session->chainIndicesBuffer.remove(c_range.first, c_range.second - c_range.first);

    session->setLastFrame(session->simulation->lastEntry());

    qDeleteAll(functions);
    session->plot->updateSimulation();

    return {a_range.second - a_range.first, c_range.second - c_range.first};
}

void PDBSimulationLayer::shift(QPair<int, int> offset)
{
    a_range.first -= offset.first;
    a_range.second -= offset.first;

    for (int i = 0; i < SERIAL_MAX; i++)
        this->offset[i] -= offset.first;

    c_range.first -= offset.second;
    c_range.second -= offset.second;

    for (int i = c_range.first; i < c_range.second; i++)
        for (int j = 0; j < session->chainCountBuffer[i]; j++)
            session->chainIndicesBuffer[i][j] -= offset.first;
}
