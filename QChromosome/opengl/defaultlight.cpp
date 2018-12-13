#include "defaultlight.h"

DefaultLight::DefaultLight(QWidget *parent) : QOpenGLWidget(parent)
{
    resize(160, 90);
}

void DefaultLight::initializeGL()
{
    initializeOpenGLFunctions();

    vaoEmpty_.create();

    sphereProgram_.create();
    sphereProgram_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/glow/vertex.glsl");
    sphereProgram_.addShaderFromSourceFile(QOpenGLShader::Geometry, ":/light/geometry.glsl");
    sphereProgram_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/light/fragment.glsl");
    sphereProgram_.link();
}

void DefaultLight::paintGL()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    vaoEmpty_.bind();
    sphereProgram_.bind();

    sphereProgram_.setUniformValue("uvScreenSize", size());
    sphereProgram_.setUniformValue("uvDefaultLight", direction);

    glDrawArrays(GL_POINTS, 0, 1);

    sphereProgram_.release();
    vaoEmpty_.release();
}

void DefaultLight::setDirection(const QVector3D& d)
{
    direction = d;
    update();
}

void DefaultLight::updateDirection(QVector2D pos)
{
    pos[1] *= -1;

    float r = pos.lengthSquared();

    if (r > 1.0)
    {
        pos.normalize();
        r = 1.0;
    }

    emit defaultLightChanged(direction = QVector3D(pos, qSqrt(1.0 - r)));
    update();
}

void DefaultLight::mousePressEvent(QMouseEvent *event)
{
    QOpenGLWidget::mousePressEvent(event);

    updateDirection(QVector2D(event->pos() - rect().center()) / qMin(width(), height()) / 0.4);
}

void DefaultLight::mouseMoveEvent(QMouseEvent *event)
{
    QOpenGLWidget::mouseMoveEvent(event);

    updateDirection(QVector2D(event->pos() - rect().center()) / qMin(width(), height()) / 0.4);
}
