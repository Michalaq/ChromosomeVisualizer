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

    // ProjectSettings;
    int PS_getFPS() const;
    void PS_setFPS(int n);

    int PS_getDocumentTime() const;
    void PS_setDocumentTime(int n);

    int PS_getMinimumTime() const;
    void PS_setMinimumTime(int n);

    int PS_getMaximumTime() const;
    void PS_setMaximumTime(int n);

    int PS_getPreviewMinTime() const;
    void PS_setPreviewMinTime(int n);

    int PS_getPreviewMaxTime() const;
    void PS_setPreviewMaxTime(int n);

    // Info
    const QString& I_getAuthor() const;
    void I_setAuthor(const QString& s);

    const QString& I_getInfo() const;
    void I_setInfo(const QString& s);

    const QString& I_getFileFormat() const;
    void I_setFileFormat(const QString& s);

    const QString& I_getFileVersion() const;
    void I_setFileVersion(const QString& s);

    void PS_read(const QJsonObject& json);
    void PS_write(QJsonObject& json) const;

    // AtomItem
    QVector<VizBallInstance> abuffer;
    bool amodified;
    bool aresized;

    // ChainItem
    QVector<std::pair<int, int>> lbuffer;

private:
    // ProjectSettings;
    int PS_FPS;
    int PS_DocumentTime;
    int PS_MinimumTime;
    int PS_MaximumTime;
    int PS_PreviewMinTime;
    int PS_PreviewMaxTime;

    // Info
    QString I_Author;
    QString I_Info;
    QString I_FileFormat;
    QString I_FileVersion;
};

#endif // SESSION_H
