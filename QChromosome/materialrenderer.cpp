#include "materialrenderer.h"
#include <cassert>

MaterialRenderer::MaterialRenderer(QWidget *parent) : QOpenGLWidget(parent)
{

}

void MaterialRenderer::initializeGL()
{
    initializeOpenGLFunctions();
}

void MaterialRenderer::paint(QPainter *painter, QRect bounds)
{
    makeCurrent();

    int s = std::min(bounds.width(), bounds.height());

    QOpenGLFramebufferObject fbo(s, s);

    assert(fbo.bind());

    //glViewport(0, 0, s, s);

    glClearColor(1.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    assert(fbo.release());

    painter->drawImage(bounds.x() + (bounds.width() - s)/2, bounds.y() + (bounds.height() - s)/2, fbo.toImage());
}
