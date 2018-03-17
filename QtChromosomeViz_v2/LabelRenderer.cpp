#include <cassert>
#include "LabelRenderer.hpp"

static QVector4D colorToVector(QColor c)
{
    return QVector4D(c.redF(), c.greenF(), c.blueF(), c.alphaF());
}

LabelRenderer::LabelRenderer(QSizeF viewportSize)
    : viewportSize_(viewportSize)
{

}

LabelRenderer::~LabelRenderer()
{

}

void LabelRenderer::initGL()
{
    initializeOpenGLFunctions();
    assert(vao_.create());

    assert(program_.create());
    program_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/label.vert");
    program_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/label.frag");
    assert(program_.link());
}

void LabelRenderer::addRef(const QString & s, unsigned int count)
{
    auto it = mapping_.find(s);
    if (it != mapping_.end())
        mapping_[s] = { it->first + count, it->second };
    else
        initializeRecord(s, count);
}

void LabelRenderer::release(const QString & s)
{
    auto it = mapping_.find(s);
    if (it != mapping_.end()) {
        if (it->first <= 1)
            mapping_.erase(it);
        else
            mapping_[s] = { it->first - 1, it->second };
    }
}

void LabelRenderer::setViewportSize(QSizeF size)
{
    viewportSize_ = size;
}

void LabelRenderer::begin()
{
    assert(program_.bind());
    vao_.bind();
}

void LabelRenderer::renderAt(QPointF at, const QString & s, QColor textColor, QColor backgroundColor)
{
    // Znajdź teksturę w pamięci podręcznej
    auto it = mapping_.find(s);
    assert(it != mapping_.end()); // Póki co zakładamy, że wszystkie odwołania będą poprawne
    auto texturePtr = it->second;
    texturePtr->bind(0);

    // Przerób koordynaty na NDC
    const double xFix = (texturePtr->width() % 2 == 0 ? 0.0 : 0.5);
    const QVector2D normalizedPos((round(at.x()) + xFix) / viewportSize_.width(), (round(at.y()) + 0.5) / viewportSize_.height());
    const QVector2D ndcPos(2.f * normalizedPos - QVector2D(1.f, 1.f));
    const QVector2D textureSize(texturePtr->width(), texturePtr->height());
    const QVector2D ndcSize(textureSize.x() / viewportSize_.width(), -textureSize.y() / viewportSize_.height());

    program_.setUniformValue("uvCenter", QVector2D(ndcPos.x(), -ndcPos.y()));
    program_.setUniformValue("uvExtents", ndcSize);
    program_.setUniformValue("sLabel", 0);
    program_.setUniformValue("ucTextColor", colorToVector(textColor));
    program_.setUniformValue("ucBackgroundColor", colorToVector(backgroundColor));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    texturePtr->release();
}

void LabelRenderer::end()
{
    vao_.release();
    program_.release();
}

void LabelRenderer::initializeRecord(const QString & s, unsigned int count)
{
    QFont font;
    QFontMetrics metrics(font);

    QImage image(metrics.width(s), metrics.height(), QImage::Format_Mono);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setPen(QPen(QBrush(Qt::white), 1.f));
    painter.drawText(0, metrics.ascent(), s);

    mapping_.insert(s, { count, QSharedPointer<QOpenGLTexture>(new QOpenGLTexture(image)) });
}
