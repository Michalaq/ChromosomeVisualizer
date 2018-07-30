#ifndef VIZWINDOW_HPP
#define VIZWINDOW_HPP

#include <QtGui>
#include <QtOpenGL>
#include <QOpenGLFunctions_4_1_Core>

#include "selection.h"
#include "pickwidget.h"
#include "treemodel.h"

class Session;

class VizWidget :   public Selection,
                    protected QOpenGLFunctions_4_1_Core,
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

    void setSession(Session* s);

signals:
    void selectionChanged(const QItemSelection&, QItemSelectionModel::SelectionFlags);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    void allocate();

    QOpenGLBuffer atomPositions_;
    QOpenGLVertexArrayObject vaoSpheres_;
    QOpenGLVertexArrayObject vaoLabels_;

    QOpenGLBuffer cameraPositions_;
    QOpenGLVertexArrayObject vaoCameras_;

    QOpenGLBuffer materials_;

    QOpenGLShaderProgram sphereProgram_;
    QOpenGLShaderProgram cylinderProgram_;
    QOpenGLShaderProgram cameraProgram_;
    QOpenGLShaderProgram labelsProgram_;
    QOpenGLShaderProgram pickingProgram_;

    GLuint buffers[2];
    GLuint picking[1];
    GLuint texture[2];
    QImage image;

    void pickSpheres();

    Session* session;
    TreeModel *model_;
    QItemSelectionModel *selectionModel_;
};

#endif /* VIZWINDOW_HPP */
