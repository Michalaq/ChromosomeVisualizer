#ifndef LABELRENDERER_HPP
#define LABELRENDERER_HPP

#include <QtCore>
#include <QtOpenGL>
#include <QOpenGLFunctions_3_3_Core>

class LabelRenderer
        : protected QOpenGLFunctions_3_3_Core
{
public:
    LabelRenderer(QSizeF viewportSize);
    ~LabelRenderer();

    void initGL();

    void addRef(const QString & s, unsigned int count = 1);
    void release(const QString & s);
    void setViewportSize(QSizeF size);

    void begin();
    void renderAt(QPointF at, const QString & s, QColor textColor, QColor backgroundColor);
    void end();

private:
    void initializeRecord(const QString & s, unsigned int count);

    QSizeF viewportSize_;
    QOpenGLVertexArrayObject vao_;
    QOpenGLShaderProgram program_;
    QMap<QString, QPair<unsigned int, QSharedPointer<QOpenGLTexture>>> mapping_;
};

#endif // LABELRENDERER_HPP
