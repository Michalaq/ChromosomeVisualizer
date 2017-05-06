#ifndef MATERIALRENDERER_H
#define MATERIALRENDERER_H

#include <QtOpenGL>

class Material;

class MaterialRenderer : public QOffscreenSurface
{
    Q_OBJECT
public:
    explicit MaterialRenderer(QScreen *targetScreen = Q_NULLPTR);

    void initializeGL();

    void paint(QPainter *painter, QRect bounds, const Material* material);

signals:

public slots:

private:
    QOpenGLVertexArrayObject vao;
    QOpenGLShaderProgram shader;
    QOpenGLContext context;
};

#endif // MATERIALRENDERER_H
