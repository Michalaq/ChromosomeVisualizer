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
    if (!(bounds.width() > 0 && bounds.height() > 0))
        return;

    context.makeCurrent(this);

    QOpenGLFramebufferObject fbo(bounds.width(), bounds.height());

    assert(fbo.bind());

    glEnable(GL_BLEND);
    //by default glBlendFunc(GL_ONE, GL_ZERO);

    QColor c;
    c = material->getColor();
    shader.setUniformValue("cColor", c.redF(), c.greenF(), c.blueF(), 1. - material->getTransparency());
    c = material->getSpecularColor();
    shader.setUniformValue("cSpecularColor", c.redF(), c.greenF(), c.blueF());
    shader.setUniformValue("fSpecularExponent", material->getSpecularExponent());

    glViewport(0, 0, bounds.width(), bounds.height());
    glDrawArrays(GL_POINTS, 0, 1);

    assert(fbo.release());

    QImage fboImage(fbo.toImage());
    QImage image(fboImage.constBits(), fboImage.width(), fboImage.height(), QImage::Format_ARGB32);

    painter->drawImage(bounds.topLeft(), image);

    context.doneCurrent();
}
