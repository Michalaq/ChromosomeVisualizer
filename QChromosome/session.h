#ifndef SESSION_H
#define SESSION_H


#include "../QtChromosomeViz_v2/bartekm_code/Simulation.h"
#include <QItemSelectionModel>

class Simulation;

#include "treeitem.h"
#include <QMatrix4x4>

struct VizCameraInstance
{
    QMatrix4x4 modelView;
    QMatrix4x4 projection;
    VizFlags flags = VisibleInEditor | VisibleInRenderer;
};

struct VizBallInstance
{
    QVector3D position;
    VizFlags flags = VisibleInEditor | VisibleInRenderer;
    QRgb color;
    QRgb specularColor;
    float specularExponent;
    float size = 1.0;
    const Material* material;
};

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
