#include "session.h"

Session::Session() :
    action(new QAction),
    simulation(new Simulation(this))
{

}

Session::~Session()
{
    delete action;
    delete simulation;
}

#include <cassert>

void Session::setFrame(std::shared_ptr<Frame> frame)
{
    assert(frame->atoms.size() == atomBuffer.get().size());

    auto& atoms = frame->atoms;

    for (int i = 0; i < atoms.size(); i++)
    {
        auto& atom = atoms[i];
        atomBuffer[i].position = QVector3D(atom.x, atom.y, atom.z);
    }
}
