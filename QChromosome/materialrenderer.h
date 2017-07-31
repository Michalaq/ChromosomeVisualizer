#ifndef MATERIALRENDERER_H
#define MATERIALRENDERER_H

#include <QtOpenGL>

class Material;

class MaterialRenderer : public QOffscreenSurface
{
    Q_OBJECT
public:
    static MaterialRenderer* getInstance();
    ~MaterialRenderer();

    void initializeGL();

    void paint(QPainter *painter, QRect bounds, const Material* material);

signals:

public slots:

private:
    explicit MaterialRenderer(QScreen *targetScreen = Q_NULLPTR);

    QOpenGLVertexArrayObject vao;
    QOpenGLShaderProgram shader;
    QOpenGLContext context;

    static MaterialRenderer* instance;
};

#endif // MATERIALRENDERER_H
