#include <cassert>
#include <cstddef>
#include "bartekm_code/PDBSimulation.h"
#include "bartekm_code/NullSimulation.h"
#include "VizWidget.hpp"

static const char * sphereVertexShaderCode =
"#version 330 core\n"
"layout(location = 0) in vec3 vVertexPosition;"
"layout(location = 1) in vec3 vVertexNormal;"
"layout(location = 2) in vec3 vInstancePosition;"
"layout(location = 3) in uint iInstanceFlags;"
"layout(location = 4) in uint iAtomID;"
"layout(location = 5) in uint cInstanceColor;"
"layout(location = 6) in float fInstanceSize;"
"uniform mat4 mvp;"
"uniform mat3 mvNormal;"
"out vec4 vPosition;"
"out vec3 vNormal;"
"flat out uint iInstanceID;"
"flat out uint iFlags;"
"out vec4 vColor;"
"vec4 colorFromARGB8(uint color) {"
    "vec4 ret;"
    "ret.a = float((color >> 24u) & 0xFFu) / 255.f;"
    "ret.r = float((color >> 16u) & 0xFFu) / 255.f;"
    "ret.g = float((color >>  8u) & 0xFFu) / 255.f;"
    "ret.b = float((color >>  0u) & 0xFFu) / 255.f;"
    "return ret;"
"}"
"void main() {"
    "vec4 pos = mvp * vec4(vVertexPosition * fInstanceSize + vInstancePosition.xyz, 1);"
    "gl_Position = pos;"
    "vPosition = pos;"
    "vNormal = normalize(mvNormal * vVertexNormal);"
    "iInstanceID = iAtomID;"
    "iFlags = iInstanceFlags;"
    "vColor = colorFromARGB8(cInstanceColor);"
"}"
;

static const char * cylinderVertexShaderCode =
"#version 330 core\n"
"layout(location = 0) in vec3 vVertexPosition;\n"
"layout(location = 1) in vec3 vVertexNormal;\n"
"layout(location = 2) in vec3 vInstancePosition;\n"
"layout(location = 3) in vec4 vInstanceRotation;\n"
"layout(location = 4) in uvec2 cInstanceColor;\n"
"layout(location = 5) in vec3 fInstanceSize;\n"
"uniform mat4 mvp;\n"
"uniform mat3 mvNormal;\n"
"out vec4 vPosition;\n"
"out vec3 vNormal;\n"
"flat out uint iInstanceID;\n"
"flat out uint iFlags;\n"
"out vec4 vColor;\n"
"vec4 colorFromARGB8(uint color) {\n"
    "vec4 ret;\n"
    "ret.a = float((color >> 24u) & 0xFFu) / 255.f;\n"
    "ret.r = float((color >> 16u) & 0xFFu) / 255.f;\n"
    "ret.g = float((color >>  8u) & 0xFFu) / 255.f;\n"
    "ret.b = float((color >>  0u) & 0xFFu) / 255.f;\n"
    "return ret;\n"
"}\n"
"vec3 rotate_vector(vec4 quat, vec3 vec) {\n"
    "return vec + 2.0 * cross(cross(vec, quat.xyz) + quat.w * vec, quat.xyz);\n"
"}\n"
"void main() {\n"
    "float blendFactor = 0.5 + 0.5 * vVertexPosition.z;\n"
    "float size = mix(fInstanceSize.x, fInstanceSize.y, blendFactor) * 0.5;"
    "vec3 pos3 = vVertexPosition * -vec3(size, size, fInstanceSize.z);\n"
    "pos3 = rotate_vector(vInstanceRotation.wxyz, pos3);\n"
    "vec4 pos = mvp * vec4(pos3 + vInstancePosition.xyz, 1);\n"
    "gl_Position = pos;\n"
    "vPosition = pos;\n"
    "vNormal = normalize(mvNormal * -rotate_vector(vInstanceRotation.wxyz, vVertexNormal));\n"
    "iInstanceID = 0u;\n"
    "iFlags = 0u;\n"
    "vColor = mix(colorFromARGB8(cInstanceColor.x | 0xFF000000U),\n"
                 "colorFromARGB8(cInstanceColor.y | 0xFF000000U),\n"
                 "blendFactor);\n"
"}\n"
;

static const char * fragmentShaderCode =
"#version 330 core\n"
"uniform vec2 uvScreenSize;"
"in vec4 vPosition;"
"in vec3 vNormal;"
"flat in uint iFlags;"
"in vec4 vColor;"
"out vec4 cColor;"
"void main() {"
    "vec2 vScreenPos = 0.5f * (vPosition.xy * uvScreenSize) / vPosition.w;"
    "float stripePhase = 0.5f * (vScreenPos.x + vScreenPos.y);"
    "float whitening = clamp(0.5f * (3.f * sin(stripePhase)), 0.f, 0.666f);"
    "vec4 baseColor = vColor;"
    "float lightness = (0.5 + 0.5 * 0.7 * (vNormal.x + vNormal.z));"
    "vec4 cDiffuse = vec4(baseColor.xyz * lightness, baseColor.a);"
    "float isSelected = ((iFlags & 4u) == 4u) ? 1.f : 0.f;"
    "cColor = mix(cDiffuse, vec4(1.f, 1.f, 1.f, 1.f), isSelected * whitening);"
"}"
;

static const char * pickingFragmentShaderCode =
"#version 330 core\n"
"flat in uint iInstanceID;"
"out vec4 cColor;"
"void main() {"
    "cColor.a = float((iInstanceID >> 24u) & 0xFFu) / 255.f;"
    "cColor.r = float((iInstanceID >> 16u) & 0xFFu) / 255.f;"
    "cColor.g = float((iInstanceID >>  8u) & 0xFFu) / 255.f;"
    "cColor.b = float((iInstanceID >>  0u) & 0xFFu) / 255.f;"
"}"
;

static const float EPSILON = 1e-4;
static const int PICKING_FRAMEBUFFER_DOWNSCALE = 1;
static const int SELECTED_FLAG = 1 << 2;

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

void VizLink::update(const QVector3D & p1, const QVector3D & p2)
{
    position = (p1 + p2) * 0.5f;
    rotation = QQuaternion::rotationTo(QVector3D(0.f, 0.f, 1.f), (p2 - p1).normalized());
    size[2] = (p1 - p2).length() * 0.5f;
}

VizWidget::VizWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , simulation_(std::make_shared<NullSimulation>())
    , needVBOUpdate_(true)
    , isSelecting_(false)
    , pickingFramebuffer_(nullptr)
    , isSelectingState_(false)
    , currentSelection_(this)
    , ballQualityParameters_(0, 0)
{

}

VizWidget::~VizWidget()
{

}

void VizWidget::initializeGL()
{
    initializeOpenGLFunctions();

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
        nullptr
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
    glVertexAttribPointer(
        6,
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
    glVertexAttribPointer(
        5,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VizLink),
        (void*)offsetof(VizLink, size)
    );

    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);

    cylinderPositions_.release();
    vaoCylinders_.release();

    planeVAO_.create();

    // TODO: Getting the first frame should be somewhere else
    setFirstFrame();

    // Shaders
    assert(sphereProgram_.create());
    sphereProgram_.addShaderFromSourceCode(QOpenGLShader::Vertex, sphereVertexShaderCode);
    sphereProgram_.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderCode);
    assert(sphereProgram_.link());

    assert(cylinderProgram_.create());
    cylinderProgram_.addShaderFromSourceCode(QOpenGLShader::Vertex, cylinderVertexShaderCode);
    cylinderProgram_.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderCode);
    assert(cylinderProgram_.link());

    assert(pickingProgram_.create());
    pickingProgram_.addShaderFromSourceCode(QOpenGLShader::Vertex,
                                           sphereVertexShaderCode);
    pickingProgram_.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                           pickingFragmentShaderCode);
    assert(pickingProgram_.link());
}

void VizWidget::paintGL()
{
    if (needVBOUpdate_)
    {
        setFrame(frameNumber_);
        needVBOUpdate_ = false;
    }

    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    generateSortedState();
    updateWholeFrameData();

    painter.beginNativePainting();

    // Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable culling
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // If there are no spheres, my driver crashes
    if (sphereCount_ > 0)
    {
        vaoCylinders_.bind();
        cylinderProgram_.bind();

        cylinderProgram_.setUniformValue("mvp", modelViewProjection_);
        cylinderProgram_.setUniformValue("mvNormal", modelViewNormal_);
        cylinderProgram_.setUniformValue("uvScreenSize",
                                (float)size().width(),
                                (float)size().height());

        glDrawArraysInstanced(GL_TRIANGLES, 0, cylinderVertCount_, sphereCount_ - 1);

        cylinderProgram_.release();
        vaoCylinders_.release();

        vaoSpheres_.bind();
        sphereProgram_.bind();

        sphereProgram_.setUniformValue("mvp", modelViewProjection_);
        sphereProgram_.setUniformValue("mvNormal", modelViewNormal_);
        sphereProgram_.setUniformValue("uvScreenSize",
                                (float)size().width(),
                                (float)size().height());

        glDrawArraysInstanced(GL_TRIANGLES, 0, sphereVertCount_, sphereCount_);

        sphereProgram_.release();
        vaoSpheres_.release();
    }

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    painter.endNativePainting();

    paintLabels(painter);

    if (isSelecting_)
    {
        QBrush brush(Qt::gray, Qt::Dense4Pattern);

        QRect r = selectionRect();
        painter.fillRect(r, brush);
        painter.setPen(Qt::white);
        painter.drawRect(r);
    }

    painter.end();
}

void VizWidget::paintLabels(QPainter & painter)
{
    const float A_LOT = 1024.f * 1024.f;
    const QPointF BIG_PT(A_LOT, A_LOT);

    painter.setPen(Qt::white);
    painter.setBrush(QBrush(Qt::white));

    for (auto it = atomLabels_.begin(); it != atomLabels_.end(); it++)
    {
        auto position = frameState_[it.key()].position;
        auto transformedPosition = modelViewProjection_ * QVector4D(position, 1.f);
        if (transformedPosition.z() >= 0.f)
        {
            QVector2D ndcPosition( transformedPosition.x() / transformedPosition.w(),
                                  -transformedPosition.y() / transformedPosition.w());
            QPointF screenPosition((float)width() * (0.5 + 0.5 * ndcPosition.x()),
                                   (float)height() * (0.5 + 0.5 * ndcPosition.y()));
            QRectF rect(screenPosition - BIG_PT, screenPosition + BIG_PT);
            painter.drawText(rect, Qt::AlignCenter, it.value());
        }
    }
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

void VizWidget::setSelectingState(bool flag)
{
    isSelectingState_ = flag;

    if (flag)
        setCursor(Qt::CrossCursor);
    else
        unsetCursor();
}

void VizWidget::setSimulation(std::shared_ptr<Simulation> dp)
{
    simulation_ = std::move(dp);
    setFirstFrame();
}

void VizWidget::advanceFrame()
{
    setFrame(frameNumber_ + 1);
}

void VizWidget::setFirstFrame()
{
    auto frame = simulation_->getFrame(0);

    sphereCount_ = frame->atoms.size();

    atomPositions_.bind();
    atomPositions_.allocate(sphereCount_ * sizeof(VizBallInstance));
    atomPositions_.release();

    cylinderPositions_.bind();
    cylinderPositions_.allocate((sphereCount_ - 1) * sizeof(VizLink));
    cylinderPositions_.release();

    selectedBitmap_.fill(false, sphereCount_);

    VizBallInstance dummy;
    dummy.color = 0xFF777777;
    dummy.size = 1.f;
    frameState_.fill(dummy, sphereCount_);

    VizLink dummy2;
    linksState_.fill(dummy2, sphereCount_ - 1);

    setFrame(0);

    // Assign appropriate default colors
    atomTypeSelection("UNB").setColor(0xFF0000);
    atomTypeSelection("BOU").setColor(0x00FF00);
    atomTypeSelection("LAM").setColor(0x0000FF);

    auto selection = atomTypeSelection("BIN");
    selection.setColor(Qt::white);
    selection.setAlpha(0.5f);

    // Run this again to update link colours
    setFrame(0);
}

void VizWidget::setFrame(frameNumber_t frame)
{
    frameNumber_ = frame;
    auto diff = simulation_->getFrame(frameNumber_);
    for (const auto & a : diff->atoms)
    {
        frameState_[a.id - 1].position = QVector3D(a.x, a.y, a.z);
        frameState_[a.id - 1].flags = 0;
        if (selectedBitmap_[a.id - 1])
            frameState_[a.id - 1].flags |= SELECTED_FLAG;
        frameState_[a.id - 1].atomID = a.id - 1;
    }

    for (int i = 0; i < linksState_.size(); i++)
    {
        auto & link = linksState_[i];

        if ((frameState_[i].color     & 0xFF000000) != 0xFF000000 ||
            (frameState_[i + 1].color & 0xFF000000) != 0xFF000000)
        {
            link.size[0] = 0.f;
            link.size[1] = 0.f;
            continue;
        }

        link.update(frameState_[i].position, frameState_[i + 1].position);
        link.color[0] = frameState_[i].color;
        link.color[1] = frameState_[i + 1].color;
        link.size[0] = frameState_[i].size;
        link.size[1] = frameState_[i + 1].size;
    }

    needVBOUpdate_ = true;
}

void VizWidget::setFrame(int frame)
{
    setFrame((frameNumber_t)frame);
    update();
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
    auto * data = (VizBallInstance*)atomPositions_.map(QOpenGLBuffer::WriteOnly);
    memcpy(data, sortedState_.constData(), sortedState_.size() * sizeof(VizBallInstance));
    atomPositions_.unmap();
    atomPositions_.release();

    cylinderPositions_.bind();
    auto * data2 = (VizLink*)cylinderPositions_.map(QOpenGLBuffer::WriteOnly);
    memcpy(data2, linksState_.constData(), linksState_.size() * sizeof(VizLink));
    cylinderPositions_.unmap();
    cylinderPositions_.release();
}

void VizWidget::mousePressEvent(QMouseEvent * event)
{
    if (!isSelectingState_)
        return event->ignore();

    isSelecting_ = true;
    selectionPoints_[0] = event->pos();
    selectionPoints_[1] = event->pos();
}

void VizWidget::mouseMoveEvent(QMouseEvent * event)
{
    if (!isSelectingState_)
        return event->ignore();

    if (isSelecting_)
    {
        QRegion r;
        r += selectionRect();
        selectionPoints_[1] = event->pos();
        r += selectionRect();
        update(r);
    }
}

void VizWidget::mouseReleaseEvent(QMouseEvent * event)
{
    if (!isSelectingState_)
        return event->ignore();

    if (isSelecting_)
    {
        isSelecting_ = false;

        const bool ctrl = event->modifiers() & Qt::ControlModifier;
        const bool shift = event->modifiers() & Qt::ShiftModifier;

        if (!(ctrl || shift))
        {
            // Clear old selection
            for (auto & sphere : selectedBitmap_)
                sphere = false;
        }

        auto oldAtoms = selectedSpheres();
        auto oldIndices = selectedSphereIndices();
        auto pickedIndices = pickSpheres();

        // New selection
        for (const auto & id : pickedIndices)
            selectedBitmap_[id] = !ctrl;

        // Now we can get the real list of all selected atoms
        QList<unsigned int> newIndices;
        for (unsigned int i = 0; i < selectedBitmap_.size(); i++)
        {
            if (selectedBitmap_[i])
                newIndices.push_back(i);
        }

        AtomSelection nuSelectionObject(newIndices, this);
        qSwap(currentSelection_, nuSelectionObject);

        // That's a hack, but it forces updating the flags
        setFrame(frameNumber_);
        update();

        emit selectionChangedIndices(newIndices, oldIndices);
        emit selectionChanged(selectedSpheres(), oldAtoms);
        emit selectionChangedObject(selectedSpheresObject());
    }
}

QList<unsigned int> VizWidget::selectedSphereIndices() const
{
    return currentSelection_.selectedIndices();
}

QList<Atom> VizWidget::selectedSpheres() const
{
    QList<Atom> ret;
    auto frame = simulation_->getFrame(frameNumber_);

    for (unsigned int i : currentSelection_.selectedIndices())
        ret.push_back(frame->atoms[i]);

    return ret;
}

AtomSelection VizWidget::selectedSpheresObject() const
{
    return currentSelection_;
}

AtomSelection VizWidget::allSelection()
{
    QList<unsigned int> l;
    for (unsigned int i = 0; i < sphereCount_; i++)
        l.push_back(i);

    return AtomSelection(l, this);
}

AtomSelection VizWidget::atomTypeSelection(const char * s)
{
    QList<unsigned int> l;
    const auto frame = simulation_->getFrame(0);
    for (unsigned int i = 0; i < sphereCount_; i++)
    {
        if (strcmp(frame->atoms[i].type, s) == 0)
            l.push_back(i);
    }

    return AtomSelection(l, this);
}

AtomSelection VizWidget::atomTypeSelection(const std::string & s)
{
    return atomTypeSelection(s.c_str());
}

void VizWidget::setVisibleSelection(AtomSelection s)
{
    for (const auto & id : currentSelection_.selectedIndices())
        selectedBitmap_[id] = false;

    auto oldAtoms = selectedSpheres();
    qSwap(currentSelection_, s);
    auto newAtoms = selectedSpheres();

    for (const auto & id : currentSelection_.selectedIndices())
        selectedBitmap_[id] = true;

    emit selectionChangedIndices(
                currentSelection_.selectedIndices(),
                s.selectedIndices());
    emit selectionChanged(selectedSpheres(), oldAtoms);
    emit selectionChangedObject(selectedSpheresObject());

    setFrame(frameNumber_);
    update();
}

void VizWidget::generateSortedState()
{
    auto sorter = [&](const VizBallInstance & a, const VizBallInstance & b) -> bool {
        float z1 = QVector4D::dotProduct(modelViewProjection_.row(2),
            QVector4D(a.position, 1.f));
        float z2 = QVector4D::dotProduct(modelViewProjection_.row(2),
            QVector4D(b.position, 1.f));
        return z1 > z2;
    };

    sortedState_ = frameState_;
    qSort(sortedState_.begin(), sortedState_.end(), sorter); // Lol xD
}

QRect VizWidget::selectionRect() const
{
    QRect r1(selectionPoints_[0], QSize(1, 1));
    QRect r2(selectionPoints_[1], QSize(1, 1));
    return r1.united(r2).intersected(geometry());
}

QList<unsigned int> VizWidget::pickSpheres()
{
    makeCurrent();

    QSize downSize(size().width() / PICKING_FRAMEBUFFER_DOWNSCALE + 1,
                   size().height() / PICKING_FRAMEBUFFER_DOWNSCALE + 1);

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
    glViewport(0, 0,
               pickingFramebuffer_->size().width(),
               pickingFramebuffer_->size().height());

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

    // Get rendered content
    // Stupid workaround for avoiding premultiplied alpha
    auto fboImage = pickingFramebuffer_->toImage();
    QImage image(fboImage.constBits(), fboImage.width(), fboImage.height(),
                 QImage::Format_ARGB32);

    QSet<unsigned int> ballIDs;

    const auto r = selectionRect();
    const QRect rscaled(r.left() / PICKING_FRAMEBUFFER_DOWNSCALE,
                        r.top() / PICKING_FRAMEBUFFER_DOWNSCALE,
                        r.width() / PICKING_FRAMEBUFFER_DOWNSCALE,
                        r.height() / PICKING_FRAMEBUFFER_DOWNSCALE);
    for (int y = rscaled.top(); y <= rscaled.bottom(); y++)
    {
        for (int x = rscaled.left(); x <= rscaled.right(); x++)
        {
            auto color = image.pixel(x, y);
            if (color != 0xFFFFFFFFU)
                ballIDs.insert(color);
        }
    }

    return ballIDs.toList();
}

AtomSelection::AtomSelection(VizWidget * widget)
    : widget_(widget)
{

}

AtomSelection::AtomSelection(QList<unsigned int> indices, VizWidget * widget)
    : selectedIndices_(std::move(indices))
    , widget_(widget)
{

}

void AtomSelection::setColor(QColor color)
{
    unsigned int code = color.rgb();
    for (unsigned int i : selectedIndices_)
    {
        auto & loc = widget_->frameState_[i].color;
        loc = (loc & 0xFF000000) | code;
    }

    widget_->needVBOUpdate_ = true;
    widget_->update();
}

void AtomSelection::setAlpha(float alpha)
{
    unsigned int code = ((unsigned int)(alpha * 255.f) << 24) & 0xFF000000U;
    for (unsigned int i : selectedIndices_)
    {
        auto & loc = widget_->frameState_[i].color;
        loc = (loc & 0x00FFFFFF) | code;
    }

    widget_->needVBOUpdate_ = true;
    widget_->update();
}

void AtomSelection::setSize(float size)
{
    for (unsigned int i : selectedIndices_)
        widget_->frameState_[i].size = size;

    widget_->needVBOUpdate_ = true;
    widget_->update();
}

void AtomSelection::setLabel(const QString & label)
{
    if (label == "")
    {
        for (unsigned int i : selectedIndices_)
            widget_->atomLabels_.remove(i);
    }
    else
    {
        for (unsigned int i : selectedIndices_)
            widget_->atomLabels_[i] = label;
    }

    widget_->update();
}

unsigned int AtomSelection::atomCount() const
{
    return selectedIndices_.size();
}

QVector3D AtomSelection::weightCenter() const
{
    if (atomCount() == 0)
        return QVector3D(0.f, 0.f, 0.f);

    QVector3D sum(0.f, 0.f, 0.f);
    auto frame = widget_->simulation_->getFrame(0);

    for (unsigned int i : selectedIndices_)
    {
        QVector3D atomPos(frame->atoms[i].x,
                          frame->atoms[i].y,
                          frame->atoms[i].z);
        sum += atomPos;
    }

    return sum /= (float)atomCount();
}

const QList<unsigned int> & AtomSelection::selectedIndices() const
{
    return selectedIndices_;
}
