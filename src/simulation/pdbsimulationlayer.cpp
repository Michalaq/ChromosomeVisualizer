#include "pdbsimulationlayer.h"
#include "messagehandler.h"
#include "session.h"

static const uint SERIAL_MAX = 100000;

PDBSimulationLayer::PDBSimulationLayer(const QString& name, Session* s, int f, int l, int t, bool b) :
    SimulationLayer(name, s, f, l, t),
    offset(new uint[SERIAL_MAX])
{
    buffer.resize(128);
    std::fill(offset, offset + SERIAL_MAX, -1);

    if (cacheHeaders(b ? 0 : INT_MAX) < 0)
        throw MessageLog({QtCriticalMsg, "No corresponding pair of MODEL/ENDMDL records was found.", file->fileName(), nullptr, -1, -1});

    makeModel();

    if (!b)
    {
        int maxTime = s->previewRange ? s->projectSettings->getPreviewMaxTime() : s->projectSettings->getMaximumTime();

        if (maxTime == s->projectSettings->getPreviewMaxTime() && maxTime < j)
            session->setLastFrame(j);
    }
}

PDBSimulationLayer::~PDBSimulationLayer()
{
    delete[] offset;
}

#include "session.h"

void PDBSimulationLayer::readEntry(int time, char* data, std::size_t stride, std::size_t pointer)
{
    if (cache.isEmpty())
        return;

    auto entry = time <= cacheHeaders(time) ? cache[time] : cache.last();

    file->seek(entry.first);

    while (file->pos() < entry.last)
    {
        file->readLine(buffer.data(), buffer.size());
        entry.line++;

        if (buffer.startsWith("ATOM  ") || buffer.startsWith("HETATM"))
        {
            bool ok;

            uint serial = buffer.mid(6, 5).trimmed().toUInt(&ok);

            if (!ok)
            {
                qcWarning("Atom serial number is malformed.", file->fileName(), entry.line, 6);
                continue;
            }

            if (offset[serial] == -1)
            {
                qcWarning("Atom serial number is undefined.", file->fileName(), entry.line, 6);
                continue;
            }

            float* position = reinterpret_cast<float*>(data + stride * offset[serial] + pointer);

            position[0] = buffer.mid(30, 8).trimmed().toFloat(&ok);

            if (!ok)
                qcWarning("Orthogonal coordinate for X is malformed.", file->fileName(), entry.line, 30);

            position[1] = buffer.mid(38, 8).trimmed().toFloat(&ok);

            if (!ok)
                qcWarning("Orthogonal coordinate for Y is malformed.", file->fileName(), entry.line, 38);

            position[2] = buffer.mid(46, 8).trimmed().toFloat(&ok);

            if (!ok)
                qcWarning("Orthogonal coordinate for Z is malformed.", file->fileName(), entry.line, 46);
        }
    }
}

bool PDBSimulationLayer::skipHeader()
{
    do
    {
        if (!file->readLine(buffer.data(), buffer.size()))
        {
            atEnd = true;
            return false;
        }

        line++;
    }
    while (!buffer.startsWith("MODEL "));

    range.first = file->pos();
    range.line = line;

    do
    {
        if (!file->readLine(buffer.data(), buffer.size()))
        {
            atEnd = true;
            return false;
        }

        line++;
    }
    while (!buffer.startsWith("ENDMDL"));

    range.last = file->pos();

    i++;

    atEnd = false;
    return true;
}

int PDBSimulationLayer::cacheHeaders(int time)
{
    if (atEnd)
        return j;

    file->seek(pos);

    while (i < first)
        if (!skipHeader())
            return j;

    while (j < time)
        if (skipHeader())
        {
            j++;
            cache.append(range);

            for (int k = 1; k < stride; k++)
            {
                if (!skipHeader())
                    return j;

                if (i > last)
                {
                    atEnd = true;
                    return j;
                }
            }
        }
        else
            return j;

    pos = file->pos();

    return j;
}

int PDBSimulationLayer::lastEntry() const
{
    return j;
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
    uint e = 0;
    QPair<int, int> range(-1, -1);

    connections = new QVector<QPair<uint, uint>>[SERIAL_MAX];
    QMap<QByteArray, QPair<ChainItem*, QMap<QByteArray, ResidueItem*>>> structure;

    auto entry = cache[0];

    file->seek(entry.first);

    while (file->pos() < entry.last)
    {
        file->readLine(buffer.data(), buffer.size());
        entry.line++;

        if (buffer.startsWith("ATOM  ") || buffer.startsWith("HETATM"))
        {
            uint serial = buffer.mid(6, 5).trimmed().toUInt(&ok);

            if (!ok)
            {
                qcWarning("Atom serial number is malformed.", file->fileName(), entry.line, 6);
                continue;
            }

            auto name = buffer.mid(12, 4).trimmed();

            auto resName = buffer.mid(17, 3).trimmed();

            auto chainID = buffer.mid(21, 1).trimmed();

            if (!structure.contains(chainID))
                structure.insert(chainID, {new ChainItem(chainID, session, model), {}});

            auto& chain = structure[chainID];

            if (!chain.second.contains(resName))
                chain.second.insert(resName, new ResidueItem(resName, session, chain.first));

            offset[serial] = (new AtomItem(serial, name, session, chain.second[resName]))->getId();

            if (range.first + range.second == offset[serial])
                range.second++;
            else
            {
                a_range.append(range);
                range = {offset[serial], 1};
            }
        }

        if (buffer.startsWith("CONECT"))
        {
            uint serial = buffer.mid(6, 5).trimmed().toUInt(&ok);

            if (!ok)
            {
                qcWarning("Atom serial number is malformed.", file->fileName(), entry.line, 6);
                continue;
            }

            if (offset[serial] == -1)
            {
                qcWarning("Atom serial number is undefined.", file->fileName(), entry.line, 6);
                continue;
            }

            for (int column : {11, 16, 21, 26})
            {
                uint bonded = buffer.mid(column, 5).trimmed().toUInt(&ok);

                if (ok)
                {
                    if (offset[bonded] == -1)
                    {
                        qcWarning("Atom serial number is undefined.", file->fileName(), entry.line, column);
                        continue;
                    }

                    if (serial < bonded)
                    {
                        connections[serial].append({e, bonded});
                        connections[bonded].append({e, serial});
                        e++;
                    }
                }
            }
        }
    }

    a_range.append(range);
    a_range.removeFirst();

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
                auto offset = session->chainBuffer.append(chain.size());

                c_range.append(offset);
                qCopy(chain.rbegin(), chain.rend(), session->chainBuffer.data() + reinterpret_cast<GLintptr>(offset) / sizeof(GLuint));
            }

            circuit.clear();
        }

    delete[] connections;
    visited.clear();
}

void PDBSimulationLayer::remove()
{
    session->simulation->removeOne(this);

    for (auto range : a_range)
        session->atomBuffer.remove(range.first, range.second);

    for (auto range : c_range)
        session->chainBuffer.remove(range);

    session->setLastFrame(session->simulation->lastEntry());

    session->plot->updateSimulation();
}
