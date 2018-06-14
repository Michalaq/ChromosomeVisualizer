#include "session.h"

Session::Session() :
    action(new QAction),
    simulation(new Simulation(this)),
    editorCamera(new Camera(this))
{

}

Session::~Session()
{
    delete action;
    delete simulation;
    delete editorCamera;
}

#include <cassert>

void Session::setFrame(std::shared_ptr<Frame> frame)
{
    assert(frame->atoms.size() == atomBuffer.size());

    auto& atoms = frame->atoms;

    for (int i = 0; i < atoms.size(); i++)
    {
        auto& atom = atoms[i];
        atomBuffer[i].position = QVector3D(atom.x, atom.y, atom.z);
    }
}
