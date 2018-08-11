#include "Simulation.h"

Simulation::Simulation(Session *s)
    : nextUnreadFrame_(0),
      model(new TreeModel(s, this)),
      session(s)
{}

Simulation::~Simulation()
{}

frameNumber_t Simulation::getFrameCount() const
{
    return nextUnreadFrame_;
}

std::shared_ptr<Frame> Simulation::getFrame(frameNumber_t position, bool *ok)
{
    Frame f = {
        0,
        0,
        std::vector<Atom>(),
        std::map<std::string, float>()
    };
    int count = 0;
    int i = 0;
    for (const auto & l : layerConcatenations_) {
        std::shared_ptr<Frame> f2 = l->getFrame(position);
        f.no = f2->no;
        f.step = f2->step;

        for (const auto& p : f2->functionValues) {
            f.functionValues[p.first + "_layer_" + std::to_string(i)] = p.second;
        }

        for (const auto & atom : f2->atoms) {
            f.atoms.push_back(atom);
            f.atoms.back().id += count;
        }

        for (const auto & connection : f2->connectedRanges) {
            f.connectedRanges.push_back({ connection.first + count, connection.second + count });
        }

        count += f2->atoms.size();
        i++;
    }
//    for (const auto& p : f.connectedRanges) {
//        std::cout << "AGGREGATED CONNECTION: " << p.first << ", " << p.second << std::endl;
//    }

    frameNumber_t nextFrame = getNextTime((position > 0) ? (position - 1) : 0) + 1;
    if (nextUnreadFrame_ < nextFrame) {
        nextUnreadFrame_ = nextFrame;
        emit frameCountChanged(nextUnreadFrame_);
    }

    if (ok) *ok = position < nextFrame;

    return std::make_shared<Frame>(f);
}

frameNumber_t Simulation::getNextTime(frameNumber_t time) const
{
    if (layerConcatenations_.empty())
        return 0;

    frameNumber_t maximum = std::numeric_limits<frameNumber_t>::min();
    frameNumber_t minimum = std::numeric_limits<frameNumber_t>::max();
    for (auto & concatenation : layerConcatenations_) {
        frameNumber_t localMinimum = concatenation->getNextTime(time);
        if (localMinimum != time)
            minimum = std::min(minimum, localMinimum);
        maximum = std::max(maximum, localMinimum);
    }

    if (minimum == std::numeric_limits<frameNumber_t>::max())
        return maximum;

    return minimum;
}

frameNumber_t Simulation::getPreviousTime(frameNumber_t time) const
{
    if (layerConcatenations_.empty())
        return 0;

    frameNumber_t maximum = std::numeric_limits<frameNumber_t>::min();
    frameNumber_t minimum = std::numeric_limits<frameNumber_t>::max();
    for (auto & concatenation : layerConcatenations_) {
        frameNumber_t localMaximum = concatenation->getPreviousTime(time);
        if (localMaximum != time)
            maximum = std::max(maximum, localMaximum);
        minimum = std::min(minimum, localMaximum);
    }

    if (maximum == std::numeric_limits<frameNumber_t>::min())
        return minimum;

    return maximum;
}

void Simulation::addSimulationLayerConcatenation(std::shared_ptr<SimulationLayerConcatenation> slc)
{
    const auto offset = getFrame(0)->atoms.size();
    layerConcatenations_.emplace_back(slc);

    int layerId = layerConcatenations_.size() - 1;
    slc->setLayerId(layerId);
   
    model->setupModelData(slc, layerId, offset);
}

TreeModel* Simulation::getModel()
{
    return model;
}

void Simulation::writePOVFrame(QTextStream &stream, frameNumber_t f)
{
    model->writePOVFrame(stream, getFrame(f));
}

#include <QVector3D>

static QTextStream& operator<<(QTextStream& out, const QVector3D & vec)
{
    return out << "<" << -vec.x() << ", " << vec.y() << ", " << vec.z() << ">";
}

void Simulation::writePOVFrames(QTextStream &stream, frameNumber_t fbeg, frameNumber_t fend)
{
    int n = getFrame(0)->atoms.size();

    QVector3D** data = new QVector3D*[n];

    for (int i = 0; i < n; i++)
        data[i] = new QVector3D[fend - fbeg + 1];

    for (int f = fbeg; f <= fend; f++)
    {
        auto frame = getFrame(f);

        for (auto a : frame->atoms)
            data[a.id - 1][f - fbeg] = {a.x, a.y, a.z};
    }

    for (int i = 0; i < n; i++)
    {
        stream << "#declare Atom" << i << "Pos = \nspline {\nnatural_spline\n";

        for (int f = fbeg; f <= fend; f++)
            stream << f << ", " << data[i][f - fbeg] << "\n";

        stream << "}\n";

        delete[] data[i];
    }

    delete[] data;

    model->writePOVFrames(stream, fbeg, fend);
}
