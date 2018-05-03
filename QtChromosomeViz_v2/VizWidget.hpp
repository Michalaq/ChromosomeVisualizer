#ifndef VIZWINDOW_HPP
#define VIZWINDOW_HPP

#include <QtGui>
#include <QtOpenGL>
#include <QOpenGLFunctions_4_2_Core>

#include "selection.h"
#include "pickwidget.h"
#include "treemodel.h"

struct shader_data_t
{
  QMatrix4x4 pro            __attribute__((aligned(16)));
  QMatrix4x4 mv             __attribute__((aligned(16)));
  QSize uvScreenSize        __attribute__((aligned(8)));
}                           __attribute__((aligned(16)));

class VizWidget :   public Selection,
                    protected QOpenGLFunctions_4_2_Core,
                    public Pickable
{
    Q_OBJECT
    Q_INTERFACES(Pickable)

public:
    VizWidget(QWidget *parent = 0);
    virtual ~VizWidget();

    virtual void initializeGL() override;
    virtual void paintGL() override;

    QPersistentModelIndex pick(const QPoint& pos);

    void setModel(TreeModel* model, QItemSelectionModel *selectionModel);
    void reloadModel();

public slots:
    void setModelView(QMatrix4x4 mat);
    void setProjection(QMatrix4x4 mat);

signals:
    void selectionChanged(const QItemSelection&, QItemSelectionModel::SelectionFlags);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

private:
    void allocate();

    QOpenGLBuffer atomPositions_;
    QOpenGLVertexArrayObject vaoSpheres_;
    QOpenGLVertexArrayObject vaoLabels_;

    QOpenGLBuffer cameraPositions_;
    QOpenGLVertexArrayObject vaoCameras_;

    QOpenGLShaderProgram sphereProgram_;
    QOpenGLShaderProgram cylinderProgram_;
    QOpenGLShaderProgram cameraProgram_;
    QOpenGLShaderProgram labelsProgram_;
    QOpenGLShaderProgram pickingProgram_;

    GLuint picking;
    GLuint texture[2];
    QImage image;

    void pickSpheres();

    TreeModel *model_;
    QItemSelectionModel *selectionModel_;
};

#endif /* VIZWINDOW_HPP */
