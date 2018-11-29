#ifndef VIZWIDGET_H
#define VIZWIDGET_H

#include <QtOpenGL>
#include <QOpenGLFunctions_4_1_Core>

#include "selection.h"
#include "pickwidget.h"

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
    QOpenGLVertexArrayObject vaoEmpty_;

    QOpenGLBuffer cameraPositions_;
    QOpenGLVertexArrayObject vaoCameras_;

    QOpenGLBuffer materials_;
    QOpenGLBuffer viewport_;

    QOpenGLShaderProgram sphereProgram_;
    QOpenGLShaderProgram cylinderProgram_;
    QOpenGLShaderProgram cameraProgram_;
    QOpenGLShaderProgram labelProgram_;
    QOpenGLShaderProgram indexProgram_;
    QOpenGLShaderProgram selectProgram_;
    QOpenGLShaderProgram glowProgram_;

    GLuint buffers[1];
    QImage image;
    QOpenGLTexture pb;
    QOpenGLTexture sb;

    void pickSpheres();

    Session* session;
};

#endif // VIZWIDGET_H
