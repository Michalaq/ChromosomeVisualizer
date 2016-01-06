#ifndef VIZWINDOW_HPP
#define VIZWINDOW_HPP

#include <memory>

#include <QtGui>
#include <QtOpenGL>
#include <QOpenGLFunctions_3_3_Core>

#include "bartekm_code/Simulation.h"

struct VizVertex
{
    QVector3D position;
    QVector3D normal;

    VizVertex rotated(const QQuaternion & q) const;
};

struct VizBallInstance
{
    QVector3D position;
    unsigned int flags;
    unsigned int atomID;
};

struct VizLink
{
    QVector3D position;
    QQuaternion rotation;

    void update(const QVector3D & p1, const QVector3D & p2);
};

using VizSegment = QPair<VizVertex, VizVertex>;

class VizWidget :   public QOpenGLWidget,
                    protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    VizWidget(QWidget *parent = 0);
    virtual ~VizWidget();

    virtual void initializeGL() override;
    virtual void paintGL() override;\

    void setSimulation(std::shared_ptr<Simulation> dp);

public slots:
    void advanceFrame();
    void setFrame(frameNumber_t frame);
    void setFrame(int frame);

    void setModelView(QMatrix4x4 mat);
    void setProjection(QMatrix4x4 mat);

    void setSelectingState(bool flag);

    // quality should be in range [0.f, 1.f]
    void setBallQuality(float quality);

signals:
    void selectionChangedIndices(const QList<unsigned int> & selected,
                                 const QList<unsigned int> & deselected);
    void selectionChanged(const QList<Atom> & selected,
                          const QList<Atom> & deselected);

protected:
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

    virtual void mousePressEvent(QMouseEvent * event) override final;
    virtual void mouseMoveEvent(QMouseEvent * event) override final;
    virtual void mouseReleaseEvent(QMouseEvent * event) override final;

    QList<unsigned int> selectedSphereIndices() const;
    QList<Atom> selectedSpheres() const;

private:
    QOpenGLBuffer sphereModel_;
    QOpenGLBuffer atomPositions_;
    QOpenGLVertexArrayObject vao_;

    QOpenGLVertexArrayObject planeVAO_;

    QOpenGLShaderProgram program_;
    QOpenGLShaderProgram planeProgram_;
    QOpenGLShaderProgram pickingProgram_;

    QMatrix4x4 projection_;
    QMatrix4x4 modelViewProjection_;
    QMatrix4x4 modelView_;
    QMatrix3x3 modelViewNormal_;

    unsigned int sphereVertCount_;
    unsigned int sphereCount_;

    std::shared_ptr<Simulation> simulation_;
    frameNumber_t frameNumber_;
    
    bool needVBOUpdate_;
    QVector<VizBallInstance> frameState_, sortedState_;
    void generateSortedState();

    bool isSelecting_;
    bool isSelectingState_;
    QPoint selectionPoints_[2];
    std::unique_ptr<QOpenGLFramebufferObject> pickingFramebuffer_;
    QList<unsigned int> selectedSphereIndices_;
    QVector<bool> selectedBitmap_;

    QRect selectionRect() const;
    QList<unsigned int> pickSpheres();

    QPair<unsigned int, unsigned int> ballQualityParameters_;
};

#endif /* VIZWINDOW_HPP */
