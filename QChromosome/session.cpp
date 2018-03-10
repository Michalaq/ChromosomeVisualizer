#include "session.h"

Session* Session::current = new Session;

Session::Session()
{
    simulation = new Simulation;
    selectionModel = new QItemSelectionModel(simulation->getModel());
}

Session::~Session()
{
    delete simulation;
    delete selectionModel;
}

void Session::setCurrent()
{
    current = this;
}
