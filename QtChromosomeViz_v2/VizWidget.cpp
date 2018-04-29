#include "VizWidget.hpp"
#include <cassert>

shader_data_t shader_data;
GLuint ubo = 0;

VizWidget::VizWidget(QWidget *parent)
    : Selection(parent)
    , pickingFramebuffer_(nullptr)
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

    assert(vaoSpheres_.create());

    assert(atomPositions_.create());
    atomPositions_.setUsagePattern(QOpenGLBuffer::DynamicDraw);

    vaoSpheres_.bind();
    atomPositions_.bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VizBallInstance),
        (void*)offsetof(VizBallInstance, position)
    );

    glEnableVertexAttribArray(1);
    glVertexAttribIPointer(
        1,
        1,
        GL_INT,
        sizeof(VizBallInstance),
        (void*)offsetof(VizBallInstance, flags)
    );

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,
        4,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        sizeof(VizBallInstance),
        (void*)offsetof(VizBallInstance, color)
    );

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(
        3,
        4,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        sizeof(VizBallInstance),
        (void*)offsetof(VizBallInstance, specularColor)
    );

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(
        4,
        1,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VizBallInstance),
        (void*)offsetof(VizBallInstance, specularExponent)
    );

    glEnableVertexAttribArray(5);
    glVertexAttribPointer(
        5,
        1,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VizBallInstance),
        (void*)offsetof(VizBallInstance, size)
    );

    atomPositions_.release();
    vaoSpheres_.release();

    assert(vaoCameras_.create());

    assert(cameraPositions_.create());
    cameraPositions_.setUsagePattern(QOpenGLBuffer::DynamicDraw);

    vaoCameras_.bind();
    cameraPositions_.bind();

    for (int i = 0; i < 4; i++)
    {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(
            i,
            4,
            GL_FLOAT,
            GL_FALSE,
            sizeof(VizCameraInstance),
            (void*)(offsetof(VizCameraInstance, modelView) + i * sizeof(QVector4D))
        );
    }

    for (int i = 0; i < 4; i++)
    {
        glEnableVertexAttribArray(i + 4);
        glVertexAttribPointer(
            i + 4,
            4,
            GL_FLOAT,
            GL_FALSE,
            sizeof(VizCameraInstance),
            (void*)(offsetof(VizCameraInstance, projection) + i * sizeof(QVector4D))
        );
    }

    glEnableVertexAttribArray(8);
    glVertexAttribIPointer(
        8,
        1,
        GL_INT,
        sizeof(VizCameraInstance),
        (void*)offsetof(VizCameraInstance, flags)
    );

    cameraPositions_.release();
    vaoCameras_.release();

    assert(vaoLabels_.create());

    vaoLabels_.bind();
    atomPositions_.bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VizBallInstance),
        (void*)offsetof(VizBallInstance, position)
    );

    glEnableVertexAttribArray(1);
    glVertexAttribIPointer(
        1,
        1,
        GL_INT,
        sizeof(VizBallInstance),
        (void*)offsetof(VizBallInstance, flags)
    );

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,
        1,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VizBallInstance),
        (void*)offsetof(VizBallInstance, size)
    );

    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(
        3,
        4,
        GL_INT,
        sizeof(VizBallInstance),
        (void*)offsetof(VizBallInstance, label)
    );

    atomPositions_.release();
    vaoLabels_.release();

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

    assert(cameraProgram_.create());
    cameraProgram_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/camera/vertex.glsl");
    cameraProgram_.addShaderFromSourceFile(QOpenGLShader::Geometry, ":/camera/geometry.glsl");
    cameraProgram_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/camera/fragment.glsl");
    assert(cameraProgram_.link());

    assert(pickingProgram_.create());
    pickingProgram_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/sphere/vertex.glsl");
    pickingProgram_.addShaderFromSourceFile(QOpenGLShader::Geometry, ":/sphere/geometry.glsl");
    pickingProgram_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/picking.frag");
    assert(pickingProgram_.link());

    assert(labelsProgram_.create());
    labelsProgram_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/labels/vertex.glsl");
    labelsProgram_.addShaderFromSourceFile(QOpenGLShader::Geometry, ":/labels/geometry.glsl");
    labelsProgram_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/labels/fragment.glsl");
    assert(labelsProgram_.link());

    AtomItem::getAtlas().initializeGL();

    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(shader_data), &shader_data, GL_DYNAMIC_DRAW);

    const GLuint binding_point_index = 0;
    glBindBufferBase(GL_UNIFORM_BUFFER, binding_point_index, ubo);

    unsigned int block_index;

    block_index = glGetUniformBlockIndex(sphereProgram_.programId(), "shader_data");
    glUniformBlockBinding(sphereProgram_.programId(), block_index, binding_point_index);

    block_index = glGetUniformBlockIndex(cylinderProgram_.programId(), "shader_data");
    glUniformBlockBinding(cylinderProgram_.programId(), block_index, binding_point_index);

    block_index = glGetUniformBlockIndex(cameraProgram_.programId(), "shader_data");
    glUniformBlockBinding(cameraProgram_.programId(), block_index, binding_point_index);

    block_index = glGetUniformBlockIndex(labelsProgram_.programId(), "shader_data");
    glUniformBlockBinding(labelsProgram_.programId(), block_index, binding_point_index);
}

#include "viewport.h"

void VizWidget::paintGL()
{
    shader_data.uvScreenSize = size();
    shader_data.ufFogDensity = viewport_->getFogDensity();
    shader_data.ufFogContribution = viewport_->getFogContribution();
    shader_data.ucFogColor = viewport_->getBackgroundColor().rgba();

    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    GLvoid* p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(p, &shader_data, sizeof(shader_data));
    glUnmapBuffer(GL_UNIFORM_BUFFER);

    // Ensure taht buffers are up to date
    allocate();

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
    if (!AtomItem::getBuffer().empty())
    {
        vaoSpheres_.bind();
        cylinderProgram_.bind();

        for (auto& strip : ChainItem::getBuffer())
            glDrawArrays(GL_LINE_STRIP, strip.first, strip.second);

        cylinderProgram_.release();
        sphereProgram_.bind();

        glDrawArrays(GL_POINTS, 0, AtomItem::getBuffer().count());

        sphereProgram_.release();
        vaoSpheres_.release();
    }

    if (CameraItem::getBuffer().count() > 1)
    {
        vaoCameras_.bind();
        cameraProgram_.bind();

        glDrawArrays(GL_POINTS, 1, CameraItem::getBuffer().count() - 1);

        cameraProgram_.release();
        vaoCameras_.release();
    }

    // If there are no spheres, my driver crashes
    if (!AtomItem::getBuffer().empty())
    {
        vaoLabels_.bind();
        labelsProgram_.bind();

        auto& atlas = AtomItem::getAtlas();

        labelsProgram_.setUniformValue("uvTextureSize",
                                (float)atlas.size().width(),
                                (float)atlas.size().height());
        labelsProgram_.setUniformValue("SampleTexture", 0);

        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, atlas.texture());

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDrawArrays(GL_POINTS, 0, AtomItem::getBuffer().count());

        labelsProgram_.release();
        vaoLabels_.release();

        glDisable(GL_BLEND);
    }

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
}

void VizWidget::setModelView(QMatrix4x4 mat)
{
    shader_data.mv = mat;

    update();
}

void VizWidget::setProjection(QMatrix4x4 mat)
{
    shader_data.pro = mat;

    update();
}

QPersistentModelIndex VizWidget::pick(const QPoint &pos)
{
    pickSpheres();

    auto color = image.pixel(pos);

    return color != 0xFFFFFFFFU ? model_->getIndices()[color] : QPersistentModelIndex();
}

void VizWidget::setModel(TreeModel* model, QItemSelectionModel *selectionModel)
{
    model_ = model;
    selectionModel_ = selectionModel;
}

void VizWidget::allocate()
{
    if (AtomItem::resized)
    {
        atomPositions_.bind();
        atomPositions_.allocate(AtomItem::getBuffer().constData(), AtomItem::getBuffer().count() * sizeof(VizBallInstance));
        atomPositions_.release();

        AtomItem::resized = false;
        AtomItem::modified = false;
    }

    if (AtomItem::modified)
    {
        atomPositions_.bind();
        atomPositions_.write(0, AtomItem::getBuffer().constData(), AtomItem::getBuffer().size() * sizeof(VizBallInstance));
        atomPositions_.release();

        AtomItem::modified = false;
    }

    if (CameraItem::resized)
    {
        cameraPositions_.bind();
        cameraPositions_.allocate(CameraItem::getBuffer().constData(), CameraItem::getBuffer().count() * sizeof(VizCameraInstance));
        cameraPositions_.release();

        CameraItem::resized = false;
        CameraItem::modified = false;
    }

    if (CameraItem::modified)
    {
        cameraPositions_.bind();
        cameraPositions_.write(0, CameraItem::getBuffer().constData(), CameraItem::getBuffer().size() * sizeof(VizCameraInstance));
        cameraPositions_.release();

        CameraItem::modified = false;
    }
}

void VizWidget::setViewport(Viewport* vp)
{
    viewport_ = vp;
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
    glEnable(GL_DEPTH_TEST);

    if (!AtomItem::getBuffer().empty())
    {
        vaoSpheres_.bind();
        pickingProgram_.bind();

        glDrawArrays(GL_POINTS, 0, AtomItem::getBuffer().count());

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

    auto indices = model_->getIndices();

    auto index = indices[image.pixel(event->pos())];
    auto material = qobject_cast<Material*>(event->source());

    if (selectionModel_ && selectionModel_->isSelected(index))
        for (auto i : selectionModel_->selectedRows())
            model_->setMaterial(i, material);
    else
        model_->setMaterial(index, material);
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

    auto& buffer = model_->getIndices();

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
