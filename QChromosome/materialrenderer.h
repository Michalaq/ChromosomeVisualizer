#ifndef MATERIALRENDERER_H
#define MATERIALRENDERER_H

#include <QtOpenGL>
#include <QOpenGLFunctions_3_3_Core>

class Material;

class MaterialRenderer : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit MaterialRenderer(QWidget *parent = 0);

    void initializeGL();

    void paint(QPainter *painter, QRect bounds, const Material* material);

signals:

public slots:

private:
    QOpenGLVertexArrayObject vao;
    QOpenGLShaderProgram shader;
};

#endif // MATERIALRENDERER_H
