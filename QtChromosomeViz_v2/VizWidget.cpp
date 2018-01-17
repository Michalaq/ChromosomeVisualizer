#include <cassert>
#include <cstddef>
#include "bartekm_code/PDBSimulationLayer.h"
#include "VizWidget.hpp"

static const float EPSILON = 1e-4;

inline static float triangleField(const QVector3D & a, const QVector3D & b, const QVector3D & c)
{
    return 0.5f * QVector3D::crossProduct(b - a, c - a).length();
}

inline static bool isDegenerate(const QVector3D & a, const QVector3D & b, const QVector3D & c)
{
    return triangleField(a, b, c) < EPSILON;
}

VizVertex VizVertex::rotated(const QQuaternion & q) const
{
    return {
        q.rotatedVector(position),
        q.rotatedVector(normal)
    };
}

void VizLink::update(const float q1[3], const float q2[3])
{
    auto p1 = QVector3D(q1[0], q1[1], q1[2]);
    auto p2 = QVector3D(q2[0], q2[1], q2[2]);
    position = (p1 + p2) * 0.5f;
    rotation = QQuaternion::rotationTo(QVector3D(0.f, 0.f, 1.f), (p2 - p1).normalized());
    size[2] = (p1 - p2).length() * 0.5f;
}

VizWidget::VizWidget(QWidget *parent)
    : Selection(parent)
    , simulation_(std::make_shared<Simulation>())
    , needVBOUpdate_(true)
    , pickingFramebuffer_(nullptr)
    , ballQualityParameters_(0, 0)
    , labelRenderer_(QSizeF(800, 600))
    , selectionModel_(nullptr)
{
    setAcceptDrops(true);
}

VizWidget::~VizWidget()
{

}

void VizWidget::initializeGL()
{
    initializeOpenGLFunctions();
    labelRenderer_.initGL();

    glEnable(GL_DEPTH_TEST);

    assert(sphereModel_.create());
    sphereModel_.setUsagePattern(QOpenGLBuffer::StaticDraw);

    assert(atomPositions_.create());
    atomPositions_.setUsagePattern(QOpenGLBuffer::DynamicDraw);

    assert(vaoSpheres_.create());

    // Create sphere model
    setBallQuality(1.f);
    vaoSpheres_.bind();

    sphereModel_.bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VizVertex),
        nullptr
    );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VizVertex),
        nullptr
    );

    sphereModel_.release();
    atomPositions_.bind();

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VizBallInstance),
        (void*)offsetof(VizBallInstance, position)
    );

    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(
        3,
        1,
        GL_UNSIGNED_INT,
        sizeof(VizBallInstance),
        (void*)offsetof(VizBallInstance, flags)
    );

    glEnableVertexAttribArray(4);
    glVertexAttribIPointer(
        4,
        1,
        GL_UNSIGNED_INT,
        sizeof(VizBallInstance),
        (void*)offsetof(VizBallInstance, atomID)
    );

    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(
        5,
        1,
        GL_UNSIGNED_INT,
        sizeof(VizBallInstance),
        (void*)offsetof(VizBallInstance, color)
    );

    glEnableVertexAttribArray(6);
    glVertexAttribIPointer(
        6,
        1,
        GL_UNSIGNED_INT,
        sizeof(VizBallInstance),
        (void*)offsetof(VizBallInstance, specularColor)
    );

    glEnableVertexAttribArray(7);
    glVertexAttribPointer(
        7,
        1,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VizBallInstance),
        (void*)offsetof(VizBallInstance, specularExponent)
    );

    glEnableVertexAttribArray(8);
    glVertexAttribPointer(
        8,
        1,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VizBallInstance),
        (void*)offsetof(VizBallInstance, size)
    );

    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);
    glVertexAttribDivisor(8, 1);

    atomPositions_.release();
    vaoSpheres_.release();

    assert(cylinderModel_.create());
    sphereModel_.setUsagePattern(QOpenGLBuffer::StaticDraw);

    assert(cylinderPositions_.create());
    atomPositions_.setUsagePattern(QOpenGLBuffer::DynamicDraw);

    assert(vaoCylinders_.create());

    // Create cylinder model
    QVector<VizVertex> cylinderVerts = generateCylinder(8);
    cylinderVertCount_ = cylinderVerts.size();
    cylinderModel_.bind();
    cylinderModel_.allocate(cylinderVerts.data(), cylinderVerts.size() * sizeof(VizVertex));
    cylinderModel_.release();

    vaoCylinders_.bind();
    cylinderModel_.bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VizVertex),
        nullptr
    );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VizVertex),
        nullptr
    );

    cylinderModel_.release();
    cylinderPositions_.bind();

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VizLink),
        nullptr
    );

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(
        3,
        4,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VizLink),
        (void*)offsetof(VizLink, rotation)
    );

    glEnableVertexAttribArray(4);
    glVertexAttribIPointer(
        4,
        2,
        GL_UNSIGNED_INT,
        sizeof(VizLink),
        (void*)offsetof(VizLink, color)
    );

    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(
        5,
        2,
        GL_UNSIGNED_INT,
        sizeof(VizLink),
        (void*)offsetof(VizLink, specularColor)
    );

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(
        6,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VizLink),
        (void*)offsetof(VizLink, specularExponent)
    );

    glEnableVertexAttribArray(7);
    glVertexAttribPointer(
        7,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VizLink),
        (void*)offsetof(VizLink, size)
    );

    glEnableVertexAttribArray(8);
    glVertexAttribIPointer(
        8,
        1,
        GL_UNSIGNED_INT,
        sizeof(VizLink),
        (void*)offsetof(VizLink, visible)
    );

    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);
    glVertexAttribDivisor(8, 1);

    cylinderPositions_.release();
    vaoCylinders_.release();

    planeVAO_.create();

    // TODO: Getting the first frame should be somewhere else
    setFirstFrame();

    // Shaders
    assert(sphereProgram_.create());
    sphereProgram_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/sphere/vertex.glsl");
    sphereProgram_.addShaderFromSourceFile(QOpenGLShader::Geometry, ":/sphere/geometry.glsl");
    sphereProgram_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/sphere/fragment.glsl");
    assert(sphereProgram_.link());

    assert(cylinderProgram_.create());
    cylinderProgram_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/cylinder/vertex.glsl");
    cylinderProgram_.addShaderFromSourceFile(QOpenGLShader::Geometry, ":/cylinder/geometry.glsl");
    cylinderProgram_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/cylinder/fragment.glsl");
    assert(cylinderProgram_.link());

    assert(pickingProgram_.create());
    pickingProgram_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/sphere/vertex.glsl");
    pickingProgram_.addShaderFromSourceFile(QOpenGLShader::Geometry, ":/sphere/geometry.glsl");
    pickingProgram_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/picking.frag");
    assert(pickingProgram_.link());
}

#include "viewport.h"

void VizWidget::paintGL()
{
    if (needVBOUpdate_)
    {
        setFrame(frameNumber_);
        needVBOUpdate_ = false;
    }

    // QPainter painter;
    // painter.begin(this);
    // painter.setRenderHint(QPainter::Antialiasing);

    generateSortedState();
    updateWholeFrameData();

    // painter.beginNativePainting();

    // Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable culling
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);

    auto backgroundColor_ = viewport_->getBackgroundColor();

    glClearColor(backgroundColor_.redF(),
                 backgroundColor_.greenF(),
                 backgroundColor_.blueF(),
                 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // If there are no spheres, my driver crashes
    if (sphereCount_ > 0)
    {
        float fogDensity_ = viewport_->getFogDensity();
        float fogContribution_ = viewport_->getFogContribution();

        vaoCylinders_.bind();
        cylinderProgram_.bind();

        cylinderProgram_.setUniformValue("mvp", modelViewProjection_);
        cylinderProgram_.setUniformValue("mv", modelView_);
        cylinderProgram_.setUniformValue("mvNormal", modelViewNormal_);
        cylinderProgram_.setUniformValue("uvScreenSize",
                                (float)size().width(),
                                (float)size().height());
        cylinderProgram_.setUniformValue("ufFogDensity", fogDensity_);
        cylinderProgram_.setUniformValue("ufFogContribution", fogContribution_);
        cylinderProgram_.setUniformValue("ucFogColor",
                                         backgroundColor_.redF(),
                                         backgroundColor_.greenF(),
                                         backgroundColor_.blueF());

        glDrawArraysInstanced(GL_TRIANGLES, 0, cylinderVertCount_, connectionCount_);

        cylinderProgram_.release();
        vaoCylinders_.release();

        vaoSpheres_.bind();
        sphereProgram_.bind();

        sphereProgram_.setUniformValue("mvp", modelViewProjection_);
        sphereProgram_.setUniformValue("mv", modelView_);
        sphereProgram_.setUniformValue("mvNormal", modelViewNormal_);
        sphereProgram_.setUniformValue("uvScreenSize",
                                (float)size().width(),
                                (float)size().height());
        sphereProgram_.setUniformValue("ufFogDensity", fogDensity_);
        sphereProgram_.setUniformValue("ufFogContribution", fogContribution_);
        sphereProgram_.setUniformValue("ucFogColor",
                                       backgroundColor_.redF(),
                                       backgroundColor_.greenF(),
                                       backgroundColor_.blueF());

        glDrawArraysInstanced(GL_TRIANGLES, 0, sphereVertCount_, sphereCount_);

        sphereProgram_.release();
        vaoSpheres_.release();
    }

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    paintLabels();
}

void VizWidget::resizeGL(int w, int h)
{
    labelRenderer_.setViewportSize(QSizeF(w, h));
}

void VizWidget::paintLabels()
{
    labelRenderer_.begin();

    auto labelTextColor_ = viewport_->getLabelTextColor();
    auto labelBackgroundColor_ = viewport_->getLabelBackgroundColor();

    for (auto it = sortedState_.begin(); it != sortedState_.end(); ++it)
    {
        auto it2 = atomLabels_.find(it->atomID);
        if (it2 == atomLabels_.end())
            continue;

        auto position = frameState_[it2.key()].position;
        auto transformedPosition = modelViewProjection_ * QVector4D(position[0], position[1], position[2], 1.f);
        if (transformedPosition.z() >= 0.f)
        {
            QVector2D ndcPosition( transformedPosition.x() / transformedPosition.w(),
                                  -transformedPosition.y() / transformedPosition.w());
            QPointF screenPosition((float)width() * (0.5 + 0.5 * ndcPosition.x()),
                                   (float)height() * (0.5 + 0.5 * ndcPosition.y()));
            labelRenderer_.renderAt(screenPosition, it2.value(),
                                    labelTextColor_, labelBackgroundColor_);
        }
    }

    labelRenderer_.end();
}

void VizWidget::setModelView(QMatrix4x4 mat)
{
    modelView_ = mat;
    modelViewNormal_ = mat.normalMatrix();
    modelViewProjection_ = projection_ * modelView_;

    needVBOUpdate_ = true;
    update();
}

void VizWidget::setProjection(QMatrix4x4 mat)
{
    projection_ = mat;
    modelViewProjection_ = projection_ * modelView_;

    needVBOUpdate_ = true;
    update();
}

void VizWidget::setSimulation(std::shared_ptr<Simulation> dp)
{
    simulation_ = std::move(dp);
    setFirstFrame();
}

QPersistentModelIndex VizWidget::pick(const QPoint &pos)
{
    pickSpheres();

    auto color = image.pixel(pos);

    return color != 0xFFFFFFFFU ? simulation_->getModel()->getIndices()[color] : QPersistentModelIndex();
}

void VizWidget::setSelectionModel(QItemSelectionModel *selectionModel)
{
    selectionModel_ = selectionModel;
}

void VizWidget::setFirstFrame()
{
    auto frame = simulation_->getFrame(0);

    sphereCount_ = frame->atoms.size();

    // Calculate connection count
    connectionCount_ = 0;
    for (const auto & conn : frame->connectedRanges)
        connectionCount_ += conn.second - conn.first;

    atomPositions_.bind();
    atomPositions_.allocate(sphereCount_ * sizeof(VizBallInstance));
    atomPositions_.release();

    cylinderPositions_.bind();
    cylinderPositions_.allocate(connectionCount_ * sizeof(VizLink));
    cylinderPositions_.release();

    VizBallInstance dummy;
    dummy.color = 0xFF777777;
    dummy.specularColor = 0xFFFFFFFF;
    dummy.specularExponent = 10.f;
    dummy.size = 1.f;
    frameState_.fill(dummy, sphereCount_);

    VizLink dummy2;
    linksState_.fill(dummy2, connectionCount_);

    setFrame(0);

    needVBOUpdate_ = true;
    update();

    // Run this again to update link colours
    setFrame(0);
}

void VizWidget::setFrame(frameNumber_t frame)
{
    frameNumber_ = frame;
    auto diff = simulation_->getFrame(frameNumber_);
    for (const auto & a : diff->atoms)
    {
        frameState_[a.id - 1].position[0] = a.x;
        frameState_[a.id - 1].position[1] = a.y;
        frameState_[a.id - 1].position[2] = a.z;
        frameState_[a.id - 1].flags = 0;
        frameState_[a.id - 1].atomID = a.id - 1;
    }

    int linkNumber = 0;
    for (const auto & conn : diff->connectedRanges)
    {
        for (int id = conn.first; id < conn.second; id++)
        {
            // linkState_ has atom IDs, which are numbered from 1, not 0
            const int i = id - 1;
            auto & link = linksState_[linkNumber++];

            /*if ((frameState_[i].color     & 0xFF000000) != 0xFF000000 ||
                (frameState_[i + 1].color & 0xFF000000) != 0xFF000000)
            {
                link.size[0] = 0.f;
                link.size[1] = 0.f;
                continue;
            }*/

            auto a = AtomItem::getBuffer()[i];
            auto b = AtomItem::getBuffer()[i + 1];

            link.update(a.position, b.position);
            link.color[0] = a.color;
            link.color[1] = b.color;
            link.specularColor[0] = a.specularColor;
            link.specularColor[1] = b.specularColor;
            link.specularExponent[0] = a.specularExponent;
            link.specularExponent[1] = b.specularExponent;
            link.size[0] = a.size;
            link.size[1] = b.size;
            link.visible = (a.flags & VISIBLE_FLAG) && (b.flags & VISIBLE_FLAG);
        }
    }

    needVBOUpdate_ = true;
}

void VizWidget::setBallQuality(float quality)
{
    static const QVector<QPair<unsigned int, unsigned int>> parameters {
        qMakePair(2u, 4u),
        qMakePair(6u, 8u),
        qMakePair(10u, 10u),
    };

    quality = std::max(0.f, std::min(.999f, quality));
    unsigned int index = int(floor(quality * parameters.size()));

    if (ballQualityParameters_ != parameters[index])
    {
        ballQualityParameters_ = parameters[index];

        QVector<VizVertex> sphereVerts = generateSphere(
                    parameters[index].first, parameters[index].second);
        sphereVertCount_ = sphereVerts.size();
        sphereModel_.bind();
        sphereModel_.allocate(sphereVerts.data(), sphereVerts.size() * sizeof(VizVertex));
        sphereModel_.release();
    }
}

QVector<VizVertex> VizWidget::generateSolidOfRevolution(
    const QVector<VizSegment> & quads,
    QVector3D axis,
    unsigned int segments
)
{
    const auto rotatePair = [](
        const QQuaternion & axis,
        const VizSegment & p
    ) -> VizSegment {
        return{ p.first.rotated(axis), p.second.rotated(axis) };
    };

    assert(segments > 2);
    assert(axis.lengthSquared() > 0.0);
    axis.normalize();

    QVector<VizVertex> ret;

    const auto pushIfNotDegenerate = [&](
        const VizVertex & a,
        const VizVertex & b,
        const VizVertex & c
    ) {
        if (!isDegenerate(a.position, b.position, c.position))
        {
            ret.push_back(a);
            ret.push_back(b);
            ret.push_back(c);
        }
    };

    for (int i = 0; i < segments; i++)
    {
        const float angle0 = ((float)i / (float)segments) * 360.0;
        const float angle1 = ((float)(i + 1) / (float)segments) * 360.0;
        const auto q0 = QQuaternion::fromAxisAndAngle(axis, angle0);
        const auto q1 = QQuaternion::fromAxisAndAngle(axis, angle1);

        // Generate quads
        for (const auto & p : quads)
        {
            const auto p0 = rotatePair(q0, p);
            const auto p1 = rotatePair(q1, p);

            pushIfNotDegenerate(p0.first, p0.second, p1.second);
            pushIfNotDegenerate(p1.second, p1.first, p0.first);
        }
    }

    return ret;
}

QVector<VizVertex> VizWidget::generateSphere(unsigned int rings, unsigned int segments)
{
    assert(rings > 1);

    const QVector3D auxAxis { 1.0, 0.0, 0.0 };
    const QVector3D mainAxis { 0.0, 0.0, 1.0 };
    const VizVertex auxVertex {
        { 0.0, 0.0, 1.0 },
        { 0.0, 0.0, 1.0 }
    };

    // Generate quads outline
    QVector<VizSegment> outline;
    for (unsigned int i = 0; i < rings; i++)
    {
        const float angle0 = ((float)i / (float)rings) * 180.0;
        const float angle1 = ((float)(i + 1) / (float)rings) * 180.0;
        const auto q0 = QQuaternion::fromAxisAndAngle(auxAxis, angle0);
        const auto q1 = QQuaternion::fromAxisAndAngle(auxAxis, angle1);

        outline.push_back({
            auxVertex.rotated(q0),
            auxVertex.rotated(q1)
        });
    }

    return generateSolidOfRevolution(outline, mainAxis, segments);
}

QVector<VizVertex> VizWidget::generateIcoSphere(unsigned int subdivisions)
{
    static const float phi = 1.6180339887498948482045868343656f;

    return {};
}

QVector<VizVertex> VizWidget::generateCylinder(unsigned int segments)
{
    const auto flatSegment = [](
        const QVector3D & a,
        const QVector3D & b,
        const QVector3D & n
    ) -> VizSegment {
        return { { a, n }, { b, n } };
    };

    static const QVector3D p0 { 0.f, 0.f, -1.f };
    static const QVector3D p1 { 0.f, 1.f, -1.f };
    static const QVector3D p2 { 0.f, 1.f, 1.f };
    static const QVector3D p3 { 0.f, 0.f, 1.f };

    static const QVector3D n0 { 0.f, 0.f, -1.f };
    static const QVector3D n1 { 0.f, 1.f, 0.f };
    static const QVector3D n2 { 0.f, 0.f, 1.f };

    static const QVector3D axis { 0.f, 0.f, 1.f };

    static const QVector<VizSegment> segs {
        flatSegment(p0, p1, n0),
        flatSegment(p1, p2, n1),
        flatSegment(p2, p3, n2),
    };

    return generateSolidOfRevolution(segs, axis, segments);
}

void VizWidget::updateWholeFrameData()
{
    atomPositions_.bind();
    atomPositions_.write(0, sortedState_.constData(), sortedState_.size() * sizeof(VizBallInstance));
    atomPositions_.release();

    cylinderPositions_.bind();
    cylinderPositions_.write(0, linksState_.constData(), linksState_.size() * sizeof(VizLink));
    cylinderPositions_.release();
}

const QMap<unsigned int, QString> & VizWidget::getLabels() const
{
    return atomLabels_;
}

void VizWidget::setViewport(Viewport* vp)
{
    viewport_ = vp;
}

void VizWidget::generateSortedState()
{
    auto sorter = [&](const VizBallInstance & a, const VizBallInstance & b) -> bool {
        float z1 = QVector4D::dotProduct(modelViewProjection_.row(2),
            QVector4D(a.position[0], a.position[1], a.position[2], 1.f));
        float z2 = QVector4D::dotProduct(modelViewProjection_.row(2),
            QVector4D(b.position[0], b.position[1], b.position[2], 1.f));
        return z1 > z2;
    };

    sortedState_ = AtomItem::getBuffer();
    qSort(sortedState_.begin(), sortedState_.end(), sorter); // Lol xD
}

void VizWidget::pickSpheres()
{
    makeCurrent();

    QSize downSize(size().width(), size().height());

    // Check if the framebuffer is large enough to
    // have whole scene rendered to it
    if (pickingFramebuffer_)
    {
        if (pickingFramebuffer_->size().width() < downSize.width() ||
            pickingFramebuffer_->size().height() < downSize.height())
        {
            pickingFramebuffer_ = nullptr;
        }
    }

    if (!pickingFramebuffer_)
    {
        pickingFramebuffer_ = std::unique_ptr<QOpenGLFramebufferObject>(
                    new QOpenGLFramebufferObject(
                        downSize, QOpenGLFramebufferObject::Depth)
                );
    }

    assert(pickingFramebuffer_->bind());
    // This is important!
    glViewport(0, pickingFramebuffer_->size().height() - downSize.height(), downSize.width(), downSize.height());

    // Render the scene with a special shader
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    if (sphereCount_ > 0)
    {
        vaoSpheres_.bind();
        pickingProgram_.bind();

        pickingProgram_.setUniformValue("mvp", modelViewProjection_);
        pickingProgram_.setUniformValue("mvNormal", modelViewNormal_);

        glDrawArraysInstanced(GL_TRIANGLES, 0, sphereVertCount_, sphereCount_);

        pickingProgram_.release();
        vaoSpheres_.release();
    }

    assert(pickingFramebuffer_->release());

    // Get rendered content avoiding premultiplied alpha
    image = pickingFramebuffer_->toImage().convertToFormat(QImage::Format_ARGB32);
}

void VizWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (qobject_cast<Material*>(event->source()))
    {
        event->acceptProposedAction();
        pickSpheres();
    }
}

void VizWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (image.pixel(event->pos()) != 0xFFFFFFFF)
        event->acceptProposedAction();
    else
        event->ignore();
}

void VizWidget::dropEvent(QDropEvent *event)
{
    event->acceptProposedAction();

    auto model = simulation_->getModel();
    auto indices = model->getIndices();

    auto index = indices[image.pixel(event->pos())];
    auto material = qobject_cast<Material*>(event->source());

    if (selectionModel_ && selectionModel_->isSelected(index))
        for (auto i : selectionModel_->selectedRows())
            model->setMaterial(i, material);
    else
        model->setMaterial(index, material);
}

void VizWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Selection::mouseReleaseEvent(event);

    pickSpheres();

    auto& p = getSelectionPath();

    auto mask = QPainterPath();
    mask.addRect(image.rect());
    mask = mask.subtracted(p);

    QPainter(&image).fillPath(mask, QColor(0xFFFFFFFFU));

    QList<unsigned> tmp;

    const auto r = p.boundingRect();
    for (int y = r.top(); y <= r.bottom(); y++)
    {
        for (int x = r.left(); x <= r.right(); x++)
        {
            auto color = image.pixel(x, y);
            if (color != 0xFFFFFFFFU)
                tmp.append(color);
        }
    }

    auto& buffer = simulation_->getModel()->getIndices();

    QItemSelection selected;

    for (auto i : tmp.toSet())
        selected.select(buffer[i], buffer[i]);

    QItemSelectionModel::SelectionFlags flags;

    auto m = event->modifiers();

    const bool ctrl = m & Qt::ControlModifier;
    const bool shift = m & Qt::ShiftModifier;

    if (!(ctrl || shift))
        flags.setFlag(QItemSelectionModel::Clear);

    flags.setFlag(ctrl ? QItemSelectionModel::Deselect : QItemSelectionModel::Select);

    emit selectionChanged(selected, flags);
}

/*void AtomSelection::setLabel(const QString & label)
{
    //TODO reimplement after changing selection
    auto& buf = widget_->simulation_->getModel()->getIndices();
    if (label == "")
    {
        for (unsigned int i : selectedIndices_)
        {
            widget_->labelRenderer_.release(widget_->atomLabels_[i]);
            widget_->atomLabels_.remove(i);
            reinterpret_cast<AtomItem*>(buf[i].internalPointer())->setLabel(label);
        }
    }
    else
    {
        widget_->labelRenderer_.addRef(label, selectedIndices_.size());

        for (unsigned int i : selectedIndices_)
        {
            widget_->labelRenderer_.release(widget_->atomLabels_[i]);
            widget_->atomLabels_[i] = label;
            reinterpret_cast<AtomItem*>(buf[i].internalPointer())->setLabel(label);
        }
    }

    widget_->update();
}

QVariant AtomSelection::getLabel() const
{
    if (selectedIndices_.isEmpty())
        return QVariant();

    QString ans = widget_->atomLabels_.value(selectedIndices_.front());

    for (auto i : selectedIndices_)
        if (widget_->atomLabels_.value(i) != ans)
            return QVariant();

    return ans;
}*/
