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
    VizWidget(QWidget *parent = Q_NULLPTR);
    virtual ~VizWidget() Q_DECL_OVERRIDE;

    virtual void initializeGL() Q_DECL_OVERRIDE;
    virtual void paintGL() Q_DECL_OVERRIDE;

    QPersistentModelIndex pick(const QPoint& pos) Q_DECL_OVERRIDE;

    void setSession(Session* s);

signals:
    void selectionChanged(const QItemSelection&, QItemSelectionModel::SelectionFlags);

protected:
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dragMoveEvent(QDragMoveEvent *event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;

    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    void allocate();

    QOpenGLBuffer cameraPositions_;
    QOpenGLBuffer atomPositions_;
    QOpenGLBuffer chainIndices_;
    QOpenGLBuffer materials_;
    QOpenGLBuffer viewport_;

    QOpenGLVertexArrayObject vaoCameras_;
    QOpenGLVertexArrayObject vaoSpheres_;
    QOpenGLVertexArrayObject vaoLabels_;
    QOpenGLVertexArrayObject vaoEmpty_;

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
