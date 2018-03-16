#ifndef SESSION_H
#define SESSION_H


#include "../QtChromosomeViz_v2/bartekm_code/Simulation.h"
#include <QItemSelectionModel>

class Simulation;

#include "treeitem.h"

class Session
{
public:
    Session();
    ~Session();

    void setFrame(std::shared_ptr<Frame> frame);

    // Session data
    Simulation* simulation;
    QItemSelectionModel *selectionModel;

    // AtomItem
    QVector<VizBallInstance> abuffer;
    bool amodified;
    bool aresized;

    // ChainItem
    QVector<std::pair<int, int>> lbuffer;
};

#endif // SESSION_H
