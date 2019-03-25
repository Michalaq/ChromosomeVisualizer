#ifndef DEFAULTLIGHT_H
#define DEFAULTLIGHT_H

#include <QtOpenGL>
#include <QOpenGLFunctions_4_0_Core>

class DefaultLight : public QOpenGLWidget, protected QOpenGLFunctions_4_0_Core
{
    Q_OBJECT
public:
    explicit DefaultLight(QWidget *parent = nullptr);

    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

    void setDirection(const QVector3D& d);

protected:
    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

signals:
    void defaultLightChanged(const QVector3D&);

public slots:

private:
    QVector3D direction;

    void updateDirection(QVector2D pos);

    QOpenGLVertexArrayObject vaoEmpty_;
    QOpenGLShaderProgram sphereProgram_;
};

#endif // DEFAULTLIGHT_H
