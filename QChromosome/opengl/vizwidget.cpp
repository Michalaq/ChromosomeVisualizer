#include "vizwidget.h"

VizWidget::VizWidget(QWidget *parent)
    : Selection(parent),
      chainIndices_(QOpenGLBuffer::IndexBuffer),
      pb(QOpenGLTexture::Target2D),
      sb(QOpenGLTexture::Target2D)
{
    setAcceptDrops(true);
}

VizWidget::~VizWidget()
{

}

#include "viewport.h"
#include "session.h"

void VizWidget::initializeGL()
{
    initializeOpenGLFunctions();

    vaoSpheres_.create();

    atomPositions_.create();
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
        1,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VizBallInstance),
        (void*)offsetof(VizBallInstance, size)
    );

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(
        3,
        1,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VizBallInstance),
        (void*)offsetof(VizBallInstance, radius)
    );

    glEnableVertexAttribArray(4);
    glVertexAttribIPointer(
        4,
        1,
        GL_INT,
        sizeof(VizBallInstance),
        (void*)offsetof(VizBallInstance, material)
    );

    atomPositions_.release();
    vaoSpheres_.release();

    vaoCameras_.create();

    cameraPositions_.create();
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

    vaoLabels_.create();

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

    vaoEmpty_.create();

    // Shaders
    sphereProgram_.create();
    sphereProgram_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/sphere/vertex.glsl");
    sphereProgram_.addShaderFromSourceFile(QOpenGLShader::Geometry, ":/sphere/geometry.glsl");
    sphereProgram_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/sphere/fragment.glsl");
    sphereProgram_.link();

    cylinderProgram_.create();
    cylinderProgram_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/cylinder/vertex.glsl");
    cylinderProgram_.addShaderFromSourceFile(QOpenGLShader::Geometry, ":/cylinder/geometry.glsl");
    cylinderProgram_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/cylinder/fragment.glsl");
    cylinderProgram_.link();

    cameraProgram_.create();
    cameraProgram_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/camera/vertex.glsl");
    cameraProgram_.addShaderFromSourceFile(QOpenGLShader::Geometry, ":/camera/geometry.glsl");
    cameraProgram_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/camera/fragment.glsl");
    cameraProgram_.link();

    labelProgram_.create();
    labelProgram_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/label/vertex.glsl");
    labelProgram_.addShaderFromSourceFile(QOpenGLShader::Geometry, ":/label/geometry.glsl");
    labelProgram_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/label/fragment.glsl");
    labelProgram_.link();

    indexProgram_.create();
    indexProgram_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/sphere/vertex.glsl");
    indexProgram_.addShaderFromSourceFile(QOpenGLShader::Geometry, ":/sphere/geometry.glsl");
    indexProgram_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/index/fragment.glsl");
    indexProgram_.link();

    selectProgram_.create();
    selectProgram_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/sphere/vertex.glsl");
    selectProgram_.addShaderFromSourceFile(QOpenGLShader::Geometry, ":/select/geometry.glsl");
    selectProgram_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/select/fragment.glsl");
    selectProgram_.link();

    glowProgram_.create();
    glowProgram_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/glow/vertex.glsl");
    glowProgram_.addShaderFromSourceFile(QOpenGLShader::Geometry, ":/glow/geometry.glsl");
    glowProgram_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/glow/fragment.glsl");
    glowProgram_.link();

    chainIndices_.create();
    chainIndices_.setUsagePattern(QOpenGLBuffer::DynamicDraw);

    materials_.create();
    materials_.setUsagePattern(QOpenGLBuffer::DynamicDraw);

    glGenBuffers(1, buffers);

    {
        glBindBuffer(GL_UNIFORM_BUFFER, buffers[0]);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(camera_data_t), &session->cameraUniformBuffer, GL_DYNAMIC_DRAW);

        const GLuint binding_point_index = 0;
        glBindBufferBase(GL_UNIFORM_BUFFER, binding_point_index, buffers[0]);

        unsigned int block_index;

        block_index = glGetUniformBlockIndex(sphereProgram_.programId(), "shader_data");
        glUniformBlockBinding(sphereProgram_.programId(), block_index, binding_point_index);

        block_index = glGetUniformBlockIndex(cylinderProgram_.programId(), "shader_data");
        glUniformBlockBinding(cylinderProgram_.programId(), block_index, binding_point_index);

        block_index = glGetUniformBlockIndex(cameraProgram_.programId(), "shader_data");
        glUniformBlockBinding(cameraProgram_.programId(), block_index, binding_point_index);

        block_index = glGetUniformBlockIndex(labelProgram_.programId(), "shader_data");
        glUniformBlockBinding(labelProgram_.programId(), block_index, binding_point_index);

        block_index = glGetUniformBlockIndex(indexProgram_.programId(), "shader_data");
        glUniformBlockBinding(indexProgram_.programId(), block_index, binding_point_index);
    }

    {
        const GLuint binding_point_index = 1;
        glBindBufferBase(GL_UNIFORM_BUFFER, binding_point_index, viewport_.bufferId());

        unsigned int block_index;

        block_index = glGetUniformBlockIndex(sphereProgram_.programId(), "viewport_data");
        glUniformBlockBinding(sphereProgram_.programId(), block_index, binding_point_index);

        block_index = glGetUniformBlockIndex(cylinderProgram_.programId(), "viewport_data");
        glUniformBlockBinding(cylinderProgram_.programId(), block_index, binding_point_index);
    }

    {
        const GLuint binding_point_index = 2;
        glBindBufferBase(GL_UNIFORM_BUFFER, binding_point_index, materials_.bufferId());

        unsigned int block_index;

        block_index = glGetUniformBlockIndex(sphereProgram_.programId(), "material_data");
        glUniformBlockBinding(sphereProgram_.programId(), block_index, binding_point_index);

        block_index = glGetUniformBlockIndex(cylinderProgram_.programId(), "material_data");
        glUniformBlockBinding(cylinderProgram_.programId(), block_index, binding_point_index);
    }
}

static const GLfloat zf = 0;
static const GLint sf = -1;

void mix(const QColor& x, const QColor& y, qreal a, GLfloat ans[4])
{
    qreal tmp[2][3];
    x.getRgbF(&tmp[0][0], &tmp[0][1], &tmp[0][2]);
    y.getRgbF(&tmp[1][0], &tmp[1][1], &tmp[1][2]);

    ans[0] = a * tmp[0][0] + (1 - a) * tmp[1][0];
    ans[1] = a * tmp[0][1] + (1 - a) * tmp[1][1];
    ans[2] = a * tmp[0][2] + (1 - a) * tmp[1][2];
    ans[3] = 1;
}

void VizWidget::paintGL()
{
    // Ensure that buffers are up to date
    allocate();

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glDrawBuffer(GL_COLOR_ATTACHMENT2);

    glClearBufferfv(GL_COLOR, 0, &zf);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Draw selection buffer
    vaoSpheres_.bind();
    selectProgram_.bind();

    glDrawArrays(GL_POINTS, 0, session->atomBuffer.count());

    selectProgram_.release();
    vaoSpheres_.release();

    GLfloat color[4];
    auto& buffer = *session->viewportUniformBuffer.constBegin();
    mix(buffer.ucFogColor, buffer.ucBackgroundColor, buffer.ubEnableFog ? buffer.ufFogStrength : 0, color);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glClearBufferfv(GL_COLOR, 0, color);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Draw cylinders and spheres
    vaoSpheres_.bind();
    cylinderProgram_.bind();
    chainIndices_.bind();

    glMultiDrawElements(GL_LINE_STRIP, session->chainCountBuffer.data(), GL_UNSIGNED_INT, session->chainOffsetsBuffer.data(), session->chainCountBuffer.count());

    chainIndices_.release();
    cylinderProgram_.release();
    sphereProgram_.bind();

    glDrawArrays(GL_POINTS, 0, session->atomBuffer.count());

    sphereProgram_.release();
    vaoSpheres_.release();

    // Draw cameras
    vaoCameras_.bind();
    cameraProgram_.bind();

    glDrawArrays(GL_POINTS, 1, session->cameraBuffer.count() - 1);

    cameraProgram_.release();
    vaoCameras_.release();

    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw selection glow
    vaoEmpty_.bind();
    glowProgram_.bind();

    glowProgram_.setUniformValue("SampleTexture", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sb.textureId());

    glDrawArrays(GL_POINTS, 0, 1);

    glowProgram_.release();
    vaoEmpty_.release();

    glDepthMask(GL_FALSE);

    // Draw labels
    vaoLabels_.bind();
    labelProgram_.bind();

    labelProgram_.setUniformValue("SampleTexture", 1);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, session->labelAtlas.textureId());

    glDrawArrays(GL_POINTS, 0, session->atomBuffer.count());

    labelProgram_.release();
    vaoLabels_.release();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

    glDisable(GL_DEPTH_TEST);
}

QPersistentModelIndex VizWidget::pick(const QPoint &pos)
{
    pickSpheres();

    int color = image.pixel(pos);

    return color != sf ? session->indices[color] : QPersistentModelIndex();
}

void VizWidget::setSession(Session *s)
{
    session = s;
}

void VizWidget::allocate()
{
    session->cameraBuffer.allocate(cameraPositions_);
    session->atomBuffer.allocate(atomPositions_);
    session->chainIndicesBuffer.allocate(chainIndices_);

    glBindBuffer(GL_UNIFORM_BUFFER, buffers[0]);
    GLvoid* p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(p, session->cameraUniformBuffer, sizeof(camera_data_t));
    glUnmapBuffer(GL_UNIFORM_BUFFER);

    session->labelAtlas.allocate();
    session->viewportUniformBuffer.allocate(viewport_);

    Material::getBuffer().allocate(materials_);
}

void VizWidget::pickSpheres()
{
    makeCurrent();

    // Ensure that buffers are up to date
    allocate();

    glEnable(GL_DEPTH_TEST);

    // Draw picking framebuffer
    glDrawBuffer(GL_COLOR_ATTACHMENT1);

    glClearBufferiv(GL_COLOR, 0, &sf);
    glClear(GL_DEPTH_BUFFER_BIT);

    vaoSpheres_.bind();
    indexProgram_.bind();

    glDrawArrays(GL_POINTS, 0, session->atomBuffer.count());

    indexProgram_.release();
    vaoSpheres_.release();

    glDisable(GL_DEPTH_TEST);

    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glReadPixels(0, 0, width(), height(), GL_RED_INTEGER, GL_INT, image.bits());

    image = image.mirrored();
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

    auto index = session->indices[image.pixel(event->pos())];
    auto material = qobject_cast<Material*>(event->source());
    auto model_ = session->simulation->getModel();
    auto selectionModel_ = session->treeView->selectionModel();

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

    auto mask = QPainterPath();
    mask.addRect(rect());
    mask = mask.subtracted(getSelectionPath());

    QPainter(&image).fillPath(mask, QColor(sf));

    int* first = (int*)image.bits();
    int* last = first + width() * height();

    std::sort(first, last);
    last = std::unique(first, last);

    if (*first == sf)
        first++;

    QMap<QPersistentModelIndex, QVector<int>> tmp;

    for (int* i = first; i != last; i++)
    {
        auto& item = session->indices[*i];
        tmp[item.parent()].append(item.row());
    }

    QItemSelection selected;

    for (auto i = tmp.begin(); i != tmp.end(); i++)
    {
        auto& parent = i.key();
        auto& list = i.value();

        qSort(list.begin(), list.end());

        int first = 0;
        int last = -1;

        for (auto j : list)
        {
            if (j != last + 1)
            {
                selected.select(parent.child(first, 0), parent.child(last, 0));
                first = j;
            }

            last = j;
        }
        selected.select(parent.child(first, 0), parent.child(last, 0));
    }

    QItemSelectionModel::SelectionFlags flags;

    auto m = event->modifiers();

    const bool ctrl = m & Qt::ControlModifier;
    const bool shift = m & Qt::ShiftModifier;

    if (!(ctrl || shift))
        flags.setFlag(QItemSelectionModel::Clear);

    flags.setFlag(ctrl ? QItemSelectionModel::Deselect : QItemSelectionModel::Select);

    emit selectionChanged(selected, flags);
}

void VizWidget::resizeEvent(QResizeEvent* event)
{
    QOpenGLWidget::resizeEvent(event);

    pb.destroy();
    pb.setSize(width(), height());
    pb.setFormat(QOpenGLTexture::R32I);
    pb.allocateStorage(QOpenGLTexture::Red_Integer, QOpenGLTexture::Int32);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, pb.textureId(), 0);

    sb.destroy();
    sb.setSize(width(), height());
    sb.setFormat(QOpenGLTexture::R32F);
    sb.allocateStorage(QOpenGLTexture::Red, QOpenGLTexture::Float32);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, sb.textureId(), 0);

    image = QImage(width(), height(), QImage::Format_RGBA8888);
}
