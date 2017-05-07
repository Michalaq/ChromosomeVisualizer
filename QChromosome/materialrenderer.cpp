#include "materialrenderer.h"
#include "material.h"
#include <cassert>

MaterialRenderer* MaterialRenderer::instance = Q_NULLPTR;

MaterialRenderer::MaterialRenderer(QScreen *targetScreen) : QOffscreenSurface(targetScreen)
{
    setFormat(QSurfaceFormat::defaultFormat());
    create();

    context.setFormat(format());
    context.create();

    initializeGL();
}

MaterialRenderer* MaterialRenderer::getInstance()
{
    return instance ? instance : (instance = new MaterialRenderer);
}

void MaterialRenderer::initializeGL()
{
    context.makeCurrent(this);

    vao.create();
    vao.bind();

    assert(shader.create());
    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/material/vertex.glsl");
    shader.addShaderFromSourceFile(QOpenGLShader::Geometry, ":/material/geometry.glsl");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/material/fragment.glsl");
    assert(shader.link());

    shader.bind();

    context.doneCurrent();
}

void MaterialRenderer::paint(QPainter *painter, QRect bounds, const Material *material)
{
    context.makeCurrent(this);

    int s = std::min(bounds.width(), bounds.height());

    QOpenGLFramebufferObject fbo(s, s);

    assert(fbo.bind());

    QColor c;
    c = material->getColor();
    shader.setUniformValue("cColor", c.redF(), c.greenF(), c.blueF(), c.alphaF());
    c = material->getSpecularColor();
    shader.setUniformValue("cSpecularColor", c.redF(), c.greenF(), c.blueF());
    shader.setUniformValue("fSpecularExponent", material->getSpecularExponent());

    glViewport(0, 0, s, s);
    glDrawArrays(GL_POINTS, 0, 1);

    assert(fbo.release());

    painter->drawImage(bounds.x() + (bounds.width() - s)/2, bounds.y() + (bounds.height() - s)/2, fbo.toImage());

    context.doneCurrent();
}
