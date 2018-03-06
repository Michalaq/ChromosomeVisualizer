#ifndef SESSION_H
#define SESSION_H


#include "../QtChromosomeViz_v2/bartekm_code/Simulation.h"

class Session
{
public:
    Session();

    std::shared_ptr<Simulation> simulation;
};

#endif // SESSION_H
