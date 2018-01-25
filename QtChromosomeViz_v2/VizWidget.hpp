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

    void setSimulation(std::shared_ptr<Simulation> dp);
    void setViewport(Viewport* vp);

    QPersistentModelIndex pick(const QPoint& pos);

    void setModel(TreeModel* model, QItemSelectionModel *selectionModel);
    void reloadModel();

public slots:
    void setModelView(QMatrix4x4 mat);
    void setProjection(QMatrix4x4 mat);

signals:
    void selectionChanged(const QItemSelection&, QItemSelectionModel::SelectionFlags);

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

    static QVector<VizVertex> generateCylinder(unsigned int segments);

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

private:
    void writeData();

    QOpenGLBuffer atomPositions_;
    QOpenGLVertexArrayObject vaoSpheres_;

    QOpenGLBuffer cylinderModel_;
    QOpenGLBuffer cylinderPositions_;
    QOpenGLVertexArrayObject vaoCylinders_;

    QOpenGLShaderProgram sphereProgram_;
    QOpenGLShaderProgram cylinderProgram_;
    QOpenGLShaderProgram pickingProgram_;

    QMatrix4x4 projection_;
    QMatrix4x4 modelViewProjection_;
    QMatrix4x4 modelView_;
    QMatrix3x3 modelViewNormal_;

    unsigned int cylinderVertCount_;

    std::unique_ptr<QOpenGLFramebufferObject> pickingFramebuffer_;

    QImage image;
    void pickSpheres();

    Viewport* viewport_;

    TreeModel *model_;
    QItemSelectionModel *selectionModel_;
};

#endif /* VIZWINDOW_HPP */
