#include "simulation.h"
#include "session.h"
#include "treemodel.h"

Simulation::Simulation(Session* s) :
    QVector<SimulationLayer*>(),
    model(new TreeModel(s)),
    session(s)
{

}

Simulation::~Simulation()
{
    qDeleteAll(*this);
    delete model;
}

void Simulation::readEntry(int time, char* data, std::size_t stride, std::size_t pointer)
{
    for (auto i : *this)
        i->readEntry(time, data, stride, pointer);
}

int Simulation::cacheHeaders(int time)
{
    int ans = -1;

    for (auto i : *this)
        ans = qMax(ans, i->cacheHeaders(time));

    return ans;
}

TreeModel* Simulation::getModel() const
{
    return model;
}

void Simulation::prepend(SimulationLayer* value)
{
    QVector<SimulationLayer*>::prepend(value);

    model->prepend(value->getModel());
}

static QTextStream& operator<<(QTextStream& out, const QVector3D& vec)
{
    return out << "<" << -vec.x() << ", " << vec.y() << ", " << vec.z() << ">";
}

#include "session.h"
#include "moviemaker.h"

void Simulation::writePOVFrames(QTextStream& stream, int fbeg, int fend)
{
    const int a_count = session->atomBuffer.count();
    const int c_count = session->chainCountBuffer.count();

    QVector3D* data = new QVector3D[(fend - fbeg + 1) * a_count];

    for (int time = fbeg; time <= fend; time++)
        readEntry(time, reinterpret_cast<char*>(data + (time - fbeg) * a_count), sizeof(QVector3D), 0);

    if (fbeg != fend)
        for (int i = 0; i < a_count; i++)
        {
            stream << "#declare Atom" << i << "Pos = \nspline {\nnatural_spline\n";

            for (int time = fbeg; time <= fend; time++)
                stream << time << ", " << data[(time - fbeg) * a_count + i] << "\n";

            stream << "}\n";
        }

    // connections
    for (int i = 0; i < c_count; i++)
        for (int j = 0; j < session->chainCountBuffer[i] - 1; j++)
        {
            uint* offset = &session->chainIndicesBuffer[i][j];

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

    // atoms
    for (int i = 0; i < a_count; i++)
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

    for (auto i : *this)
        ans = qMax(ans, i->lastEntry());

    return ans;
}
