#include "simulation.h"
#include "session.h"
#include "treemodel.h"

Simulation::Simulation(Session* s) :
    QVector<SimulationLayer*>(),
    model(new TreeModel(s))
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

void Simulation::writePOVFrames(QTextStream& stream, int fbeg, int fend)
{
    int a_count = model->atomCount();

    QVector3D* data = new QVector3D[(fend - fbeg + 1) * a_count];

    for (int time = fbeg; time <= fend; time++)
        readEntry(time, reinterpret_cast<char*>(data + (time - fbeg) * a_count), sizeof(QVector3D), 0);

    for (int i = 0; i < a_count; i++)
    {
        stream << "#declare Atom" << i << "Pos = \nspline {\nnatural_spline\n";

        for (int time = fbeg; time <= fend; time++)
            stream << time << ", " << data[(time - fbeg) * a_count + i] << "\n";

        stream << "}\n";
    }

    if (fbeg == fend)
        model->writePOVFrame(stream, data);
    else
        model->writePOVFrames(stream, fbeg, fend);

    delete[] data;
}
