#ifndef SESSION_H
#define SESSION_H


#include "glbuffer.h"
#include <QAction>
#include "treeitem.h"
#include "../QtChromosomeViz_v2/bartekm_code/Simulation.h"
#include "camera.h"
#include "labelatlas.h"
#include "treeview.h"
#include "namedelegate.h"
#include "visibilitydelegate.h"
#include "tagsdelegate.h"
#include "projectsettings.h"
#include "viewport.h"
#include "materialbrowser.h"

class Session
{
public:
    Session();
    ~Session();

    QAction *action;
    Simulation *simulation;

    GLBuffer<VizCameraInstance> cameraBuffer;
    GLBuffer<VizBallInstance> atomBuffer;
    QVector<std::pair<int, int>> chainBuffer;

    camera_data_t cameraUniformBuffer;

    LabelAtlas labelAtlas;

    Camera* editorCamera;
    Camera* currentCamera;
    QList<Camera*> userCameras;

    TreeView* treeView;
    ListView* listView;

    ProjectSettings* projectSettings;
    Viewport* viewport;

    void setFrame(int n);

    void fromJson(const QJsonDocument& json);
    QJsonDocument toJson() const;

    void reallocateBuffers();

    bool openProject();
    void saveProject() const;
    void saveProjectAs() const;

private:
    NameDelegate *nd;
    VisibilityDelegate *vd;
    TagsDelegate *td;
    MaterialDelegate *md;
};

#endif // SESSION_H
