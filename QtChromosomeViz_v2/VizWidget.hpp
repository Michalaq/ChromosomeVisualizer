#ifndef VIZWINDOW_HPP
#define VIZWINDOW_HPP

#include <memory>

#include <QtGui>
#include <QtOpenGL>
#include <QOpenGLFunctions_3_3_Core>

#include "selection.h"
#include "pickwidget.h"
#include "treemodel.h"

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
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

private:
    void writeData();

    QOpenGLBuffer atomPositions_;
    QOpenGLVertexArrayObject vaoSpheres_;

    QOpenGLShaderProgram sphereProgram_;
    QOpenGLShaderProgram cylinderProgram_;
    QOpenGLShaderProgram pickingProgram_;

    QMatrix4x4 projection_;
    QMatrix4x4 modelView_;
    QMatrix3x3 modelViewNormal_;

    std::unique_ptr<QOpenGLFramebufferObject> pickingFramebuffer_;

    QImage image;
    void pickSpheres();

    Viewport* viewport_;

    TreeModel *model_;
    QItemSelectionModel *selectionModel_;
};

#endif /* VIZWINDOW_HPP */
