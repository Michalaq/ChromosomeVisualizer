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
#include "mediapanel.h"

class Session : public QObject
{
    Q_OBJECT

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

    MediaPanel* mediaPanel;

    void fromJson(const QJsonDocument& json);
    QJsonDocument toJson() const;

    void reallocateBuffers();
    void changeCamera(Camera* camera);

    bool openProject();
    void saveProject() const;
    void saveProjectAs() const;

    void setFPS(int fps);
    void setDocumentTime(int time);
    void setMinimumTime(int time);
    void setMaximumTime(int time);
    void setPreviewMinTime(int time);
    void setPreviewMaxTime(int time);
    void setLastFrame(int time);

private:
    NameDelegate *nd;
    VisibilityDelegate *vd;
    TagsDelegate *td;
    MaterialDelegate *md;

    int lastFrame;

signals:
    void documentTimeChanged(int);
};

#endif // SESSION_H
