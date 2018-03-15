#include "session.h"

Session::Session()
{
    simulation = new Simulation(this);
    selectionModel = new QItemSelectionModel(simulation->getModel());
}

Session::~Session()
{
    delete simulation;
    delete selectionModel;
}

#include <cassert>

void Session::setFrame(std::shared_ptr<Frame> frame)
{
    assert(frame->atoms.size() == abuffer.size());

    auto& atoms = frame->atoms;

    for (int i = 0; i < atoms.size(); i++)
    {
        auto& atom = atoms[i];
        abuffer[i].position = QVector3D(atom.x, atom.y, atom.z);
    }

    amodified = true;
}
