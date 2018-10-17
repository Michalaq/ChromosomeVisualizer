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
#include "plot.h"
#include "tabwidget.h"
#include "simulation/pdbsimulationlayser.h"

class MainWindow;

class Session : public QObject
{
    Q_OBJECT
    Q_ENUMS(PlayMode)

public:
    Session(MainWindow* w);
    ~Session();

    QAction *action;
    Simulation *simulation;
    PDBSimulationLayerV2 *foo;

    QVector<QPersistentModelIndex> indices;

    GLBuffer<VizCameraInstance> cameraBuffer;
    GLBuffer<VizBallInstance> atomBuffer;
    QVector<int> chainBuffer[2];

    camera_data_t* cameraUniformBuffer;

    LabelAtlas labelAtlas;

    ProjectSettings* projectSettings;
    TabWidget* renderSettings;

    Camera* editorCamera;
    Camera* currentCamera;
    QList<Camera*> userCameras;
    QVector3D origin;

    TreeView* treeView;
    ListView* listView;

    Viewport* viewport;

    MediaPanel* mediaPanel;

    Plot* plot;

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

    bool autokeying;
    bool playForwards;
    bool playBackwards;

    bool previewRange;

    enum PlayMode {
        PM_Simple,
        PM_Cycle,
        PM_PingPong
    };

    PlayMode playMode;

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
