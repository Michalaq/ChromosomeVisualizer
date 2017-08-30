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

MaterialRenderer::~MaterialRenderer()
{

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

#include <fstream>

void MaterialRenderer::paint(QPainter *painter, QRect bounds, const Material *material)
{
    if (!(bounds.width() > 0 && bounds.height() > 0))
        return;

    std::ofstream file("sphere.pov");

    file << *material
         << "plane {z, 1000 texture{pigment {gradient <1,1,0> color_map {[0.0 color rgb<0.4, 0.4, 0.4>] [0.5 color rgb<0.4, 0.4, 0.4>] [0.5 color rgb<0.6, 0.6, 0.6>  ] [1.0 color rgb<0.6, 0.6, 0.6>  ] } scale 500 translate 0}}}\n"
         << "camera {perspective location <0, 0, -5> look_at <0, 0, 0>}\n"
         << "sphere {<0, 0, 0>, 2 texture { m" << QString::number(quintptr(material), 16).toStdString() << " }}\n"
         << "light_source {<1, 1, -2> color rgb<1, 1, 1> parallel point_at <0,0,0>}\n";

    file.close();

    QStringList argv;
    argv << QString("+W") + QString::number(bounds.width())
         << QString("+H") + QString::number(bounds.height())
         << "-GA"
         << "-D"
         << "-O-"
         << "sphere.pov";

    QProcess p;

    p.start("povray", argv);
    p.waitForFinished();

    QImage img;
    img.loadFromData(p.readAllStandardOutput(), "PNG");

    painter->drawImage(bounds.topLeft(), img);
}
