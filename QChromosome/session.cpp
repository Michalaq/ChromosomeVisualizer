#include "session.h"

Session::Session()
{
    simulation = std::make_shared<Simulation>();
    selectionModel = new QItemSelectionModel(simulation->getModel());
}
