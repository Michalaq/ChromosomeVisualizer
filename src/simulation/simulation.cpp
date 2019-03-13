#include "simulation.h"
#include "session.h"
#include "treemodel.h"

Simulation::Simulation(Session* s) :
    model(new TreeModel(s)),
    session(s)
{

}

Simulation::~Simulation()
{
    qDeleteAll(layers);
    qDeleteAll(series);
    delete model;
}

void Simulation::readEntry(int time, char* data, std::size_t stride, std::size_t pointer)
{
    for (auto i : layers)
        i->readEntry(time, data, stride, pointer);

    for (auto i : series)
        i->cacheHeaders(time);
}

int Simulation::cacheHeaders(int time)
{
    int ans = -1;

    for (auto i : layers)
        ans = qMax(ans, i->cacheHeaders(time));

    for (auto i : series)
        ans = qMax(ans, i->cacheHeaders(time));

    return ans;
}

TreeModel* Simulation::getModel() const
{
    return model;
}

void Simulation::prepend(SimulationLayer* value)
{
    layers.prepend(value);
    model->prepend(value->getModel());
}

void Simulation::prepend(SimulationSeries* value)
{
    series.prepend(value);
}

void Simulation::removeOne(SimulationLayer* layer)
{
    layers.removeOne(layer);
}

void Simulation::removeOne(SimulationSeries* value)
{
    series.removeOne(value);
}

#include "messagehandler.h"
#include <QJsonArray>

void Simulation::read(const QJsonArray& json)
{
    for (const auto& object : json)
        try
        {
            series.append(SimulationSeries::read(object.toObject(), session));
        }
        catch (const MessageLog& log)
        {
            MessageHandler::getInstance()->handleMessage(log.type, log.description, log.file, log.line, log.column);
        }
}

void Simulation::write(QJsonArray& json) const
{
    for (auto s : series)
    {
        QJsonObject object;
        s->write(object);
        json.append(object);
    }
}

static QTextStream& operator<<(QTextStream& out, const QVector3D& vec)
{
    return out << "<" << -vec.x() << ", " << vec.y() << ", " << vec.z() << ">";
}

#include "session.h"
#include "moviemaker.h"

void Simulation::writePOVFrames(QTextStream& stream, int fbeg, int fend)
{
    const int a_count = session->atomBuffer.buffer.size();

    QVector3D* data = new QVector3D[(fend - fbeg + 1) * a_count];

    for (int time = fbeg; time <= fend; time++)
        readEntry(time, reinterpret_cast<char*>(data + (time - fbeg) * a_count), sizeof(QVector3D), 0);

    if (fbeg != fend)
        for (int j = 0; j < session->atomBuffer.firsts.size(); j++)
            for (int i = session->atomBuffer.firsts[j]; i < session->atomBuffer.firsts[j] + session->atomBuffer.counts[j]; i++)
            {
                stream << "#declare Atom" << i << "Pos = \nspline {\nnatural_spline\n";

                for (int time = fbeg; time <= fend; time++)
                    stream << time << ", " << data[(time - fbeg) * a_count + i] << "\n";

                stream << "}\n";
            }

    // connections
    for (int i = 0; i < session->chainBuffer.firsts.size(); i++)
    {
        GLuint* offset = session->chainBuffer.data() + reinterpret_cast<GLintptr>(session->chainBuffer.firsts[i]) / sizeof(GLuint);

        for (int j = 0; j < session->chainBuffer.counts[i] - 1; j++, offset++)
        {
            const auto& first = session->atomBuffer[offset[0]];
            const auto& second = session->atomBuffer[offset[1]];

            if ((first.flags & second.flags).testFlag(VisibleInRenderer))
            {
                if (fbeg == fend)
                    MovieMaker::addCylinder(stream, data[offset[0]], data[offset[1]], first.size / 2, second.size / 2, first.material, second.material);
                else
                    MovieMaker::addCylinder1(stream, offset[0], offset[1], first.size / 2, second.size / 2, first.material, second.material);
            }
        }
    }

    // atoms
    for (int j = 0; j < session->atomBuffer.firsts.size(); j++)
        for (int i = session->atomBuffer.firsts[j]; i < session->atomBuffer.firsts[j] + session->atomBuffer.counts[j]; i++)
        {
            const auto& atom = session->atomBuffer[i];

            if (atom.flags.testFlag(VisibleInRenderer))
            {
                if (fbeg == fend)
                    MovieMaker::addSphere(stream, data[i], atom.size, atom.material);
                else
                    MovieMaker::addSphere1(stream, i, atom.size, atom.material);
            }
        }

    delete[] data;
}

int Simulation::lastEntry() const
{
    int ans = 0;

    for (auto i : layers)
        ans = qMax(ans, i->lastEntry());

    for (auto i : series)
        ans = qMax(ans, i->lastEntry());

    return ans;
}
