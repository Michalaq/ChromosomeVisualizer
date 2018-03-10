#ifndef SESSION_H
#define SESSION_H


#include "../QtChromosomeViz_v2/bartekm_code/Simulation.h"
#include <QItemSelectionModel>

class Session
{
public:
    Session();
    ~Session();

    void setCurrent();

    static Session* current;

    // Session data
    Simulation* simulation;
    QItemSelectionModel *selectionModel;
};

#endif // SESSION_H
