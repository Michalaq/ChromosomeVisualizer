#ifndef SESSION_H
#define SESSION_H


#include "../QtChromosomeViz_v2/bartekm_code/Simulation.h"
#include <QItemSelectionModel>

class Session
{
public:
    Session();

    std::shared_ptr<Simulation> simulation;
    QItemSelectionModel *selectionModel;
};

#endif // SESSION_H
