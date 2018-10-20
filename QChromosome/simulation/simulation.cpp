#include "simulation.h"
#include "session.h"
#include "treemodel.h"

SimulationV2::SimulationV2(Session* s) :
    QVector<SimulationLayerV2*>(),
    model(new TreeModel(s))
{

}

SimulationV2::~SimulationV2()
{
    qDeleteAll(*this);
    delete model;
}

void SimulationV2::readEntry(int time, char* data, std::size_t stride, std::size_t pointer)
{
    for (auto i : *this)
        i->readEntry(time, data, stride, pointer);
}

int SimulationV2::cacheHeaders(int time)
{
    int ans = -1;

    for (auto i : *this)
        ans = qMax(ans, i->cacheHeaders(time));

    return ans;
}

TreeModel* SimulationV2::getModel() const
{
    return model;
}

void SimulationV2::prepend(SimulationLayerV2* value)
{
    QVector<SimulationLayerV2*>::prepend(value);

    model->prepend(value->getModel());
}

static QTextStream& operator<<(QTextStream& out, const QVector3D& vec)
{
    return out << "<" << -vec.x() << ", " << vec.y() << ", " << vec.z() << ">";
}

void SimulationV2::writePOVFrames(QTextStream& stream, int fbeg, int fend)
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
