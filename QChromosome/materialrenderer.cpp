#include "materialrenderer.h"
#include <cassert>

MaterialRenderer::MaterialRenderer(QWidget *parent) : QOpenGLWidget(parent)
{

}

void MaterialRenderer::initializeGL()
{
    initializeOpenGLFunctions();

    vao.create();
    vao.bind();

    assert(shader.create());
    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/material/material/material.vert");
    shader.addShaderFromSourceFile(QOpenGLShader::Geometry, ":/material/material/material.geom");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/material/material/material.frag");
    assert(shader.link());

    shader.bind();
}

void MaterialRenderer::paint(QPainter *painter, QRect bounds)
{
    makeCurrent();

    int s = std::min(bounds.width(), bounds.height());

    QOpenGLFramebufferObject fbo(s, s);

    assert(fbo.bind());

    glViewport(0, 0, s, s);

    glDrawArrays(GL_POINTS, 0, 1);

    assert(fbo.release());

    painter->drawImage(bounds.x() + (bounds.width() - s)/2, bounds.y() + (bounds.height() - s)/2, fbo.toImage());
}
