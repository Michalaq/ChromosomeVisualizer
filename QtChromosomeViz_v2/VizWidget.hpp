#ifndef VIZWINDOW_HPP
#define VIZWINDOW_HPP

#include <memory>

#include <QtGui>
#include <QtOpenGL>
#include <QOpenGLFunctions_3_3_Core>

#include "bartekm_code/Simulation.h"
#include "LabelRenderer.hpp"

struct VizVertex
{
    QVector3D position;
    QVector3D normal;

    VizVertex rotated(const QQuaternion & q) const;
};

struct VizLink
{
    QVector3D position;
    QQuaternion rotation;
    unsigned int color[2];
    unsigned int specularColor[2];
    float specularExponent[2];
    float size[3];
    unsigned int visible;

    void update(const float q1[3], const float q2[3]);
};

using VizSegment = QPair<VizVertex, VizVertex>;

#include "selection.h"
#include "pickwidget.h"
class Viewport;

class VizWidget :   public Selection,
                    protected QOpenGLFunctions_3_3_Core,
                    public Pickable
{
    Q_OBJECT
    Q_INTERFACES(Pickable)

public:
    VizWidget(QWidget *parent = 0);
    virtual ~VizWidget();

    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;

    void setSimulation(std::shared_ptr<Simulation> dp);
    void setViewport(Viewport* vp);

    QPersistentModelIndex pick(const QPoint& pos);

    void setSelectionModel(QItemSelectionModel *selectionModel);

public slots:
    void setFrame(frameNumber_t frame);

    void setModelView(QMatrix4x4 mat);
    void setProjection(QMatrix4x4 mat);

    // quality should be in range [0.f, 1.f]
    void setBallQuality(float quality);

    const QMap<unsigned int, QString> & getLabels() const;

    void writePOVFrame(std::ostream& stream, frameNumber_t f) const;
    void writePOVFrames(std::ostream& stream, frameNumber_t fbeg, frameNumber_t fend) const;

signals:
    void selectionChanged(const QItemSelection&, QItemSelectionModel::SelectionFlags);

protected:
    void paintLabels();

    // Generates vertices for a solid of revolution based on the given outline.
    //   quads - line segments disjoint from the axis of rotation
    //   axis - axis of rotation. Must not be zero.
    //   segments - number of segments to be generated.
    static QVector<VizVertex> generateSolidOfRevolution(
        const QVector<VizSegment> & quads,
        QVector3D axis,
        unsigned int segments
    );

    static QVector<VizVertex> generateSphere(unsigned int rings, unsigned int segments);
    static QVector<VizVertex> generateIcoSphere(unsigned int subdivisions);
    static QVector<VizVertex> generateCylinder(unsigned int segments);

    void setFirstFrame();
    void updateWholeFrameData();

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

private:
    QOpenGLBuffer sphereModel_;
    QOpenGLBuffer atomPositions_;
    QOpenGLVertexArrayObject vaoSpheres_;

    QOpenGLBuffer cylinderModel_;
    QOpenGLBuffer cylinderPositions_;
    QOpenGLVertexArrayObject vaoCylinders_;

    QOpenGLVertexArrayObject planeVAO_;

    QOpenGLShaderProgram sphereProgram_;
    QOpenGLShaderProgram cylinderProgram_;
    QOpenGLShaderProgram pickingProgram_;

    QMatrix4x4 projection_;
    QMatrix4x4 modelViewProjection_;
    QMatrix4x4 modelView_;
    QMatrix3x3 modelViewNormal_;

    unsigned int sphereVertCount_;
    unsigned int cylinderVertCount_;
    unsigned int sphereCount_;
    unsigned int connectionCount_;

    std::shared_ptr<Simulation> simulation_;
    frameNumber_t frameNumber_;

    bool needVBOUpdate_;
    QVector<VizBallInstance> frameState_, sortedState_;
    QVector<VizLink> linksState_;
    void generateSortedState();

    std::unique_ptr<QOpenGLFramebufferObject> pickingFramebuffer_;

    QImage image;
    void pickSpheres();

    QPair<unsigned int, unsigned int> ballQualityParameters_;

    QMap<unsigned int, QString> atomLabels_;

    LabelRenderer labelRenderer_;

    Viewport* viewport_;

    QVector<const Material*> materials;

    QItemSelectionModel *selectionModel_;
};

#endif /* VIZWINDOW_HPP */
