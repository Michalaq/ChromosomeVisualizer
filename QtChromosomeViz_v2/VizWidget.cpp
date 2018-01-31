#include "VizWidget.hpp"
#include <cassert>

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
        GL_UNSIGNED_INT,
        sizeof(VizBallInstance),
        (void*)offsetof(VizBallInstance, flags)
    );

    glEnableVertexAttribArray(2);
    glVertexAttribIPointer(
        2,
        1,
        GL_UNSIGNED_INT,
        sizeof(VizBallInstance),
        (void*)offsetof(VizBallInstance, atomID)
    );

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(
        3,
        4,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        sizeof(VizBallInstance),
        (void*)offsetof(VizBallInstance, color)
    );

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(
        4,
        4,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        sizeof(VizBallInstance),
        (void*)offsetof(VizBallInstance, specularColor)
    );

    glEnableVertexAttribArray(5);
    glVertexAttribPointer(
        5,
        1,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VizBallInstance),
        (void*)offsetof(VizBallInstance, specularExponent)
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

    atomPositions_.release();
    vaoSpheres_.release();

    assert(vaoCameras_.create());

    assert(cameraPositions_.create());
    cameraPositions_.setUsagePattern(QOpenGLBuffer::DynamicDraw);

    vaoCameras_.bind();
    cameraPositions_.bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        16,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VizCameraInstance),
        (void*)offsetof(VizCameraInstance, modelView)
    );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        16,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VizCameraInstance),
        (void*)offsetof(VizCameraInstance, projection)
    );

    cameraPositions_.release();
    vaoCameras_.release();

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
}

#include "viewport.h"

void VizWidget::paintGL()
{
    writeData();

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
        float fogDensity_ = viewport_->getFogDensity();
        float fogContribution_ = viewport_->getFogContribution();

        vaoSpheres_.bind();
        cylinderProgram_.bind();

        cylinderProgram_.setUniformValue("pro", projection_);
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

        for (auto& strip : ChainItem::getBuffer())
            glDrawArrays(GL_LINE_STRIP, strip.first, strip.second);

        cylinderProgram_.release();
        sphereProgram_.bind();

        sphereProgram_.setUniformValue("pro", projection_);
        sphereProgram_.setUniformValue("mv", modelView_);
        sphereProgram_.setUniformValue("uvScreenSize",
                                (float)size().width(),
                                (float)size().height());
        sphereProgram_.setUniformValue("ufFogDensity", fogDensity_);
        sphereProgram_.setUniformValue("ufFogContribution", fogContribution_);
        sphereProgram_.setUniformValue("ucFogColor",
                                       backgroundColor_.redF(),
                                       backgroundColor_.greenF(),
                                       backgroundColor_.blueF());

        glDrawArrays(GL_POINTS, 0, AtomItem::getBuffer().count());

        sphereProgram_.release();
        vaoSpheres_.release();
    }

    if (!CameraItem::getBuffer().empty())
    {
        vaoCameras_.bind();
        cameraProgram_.bind();

        cameraProgram_.setUniformValue("pro", projection_);
        cameraProgram_.setUniformValue("mv", modelView_);

        glDrawArrays(GL_POINTS, 0, CameraItem::getBuffer().count());

        cameraProgram_.release();
        vaoCameras_.release();
    }

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
}

/*void VizWidget::paintLabels()
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
}*/

void VizWidget::setModelView(QMatrix4x4 mat)
{
    modelView_ = mat;
    modelViewNormal_ = mat.normalMatrix();

    update();
}

void VizWidget::setProjection(QMatrix4x4 mat)
{
    projection_ = mat;

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

    reloadModel();
}

void VizWidget::reloadModel()
{
    atomPositions_.bind();
    atomPositions_.allocate(AtomItem::getBuffer().count() * sizeof(VizBallInstance));
    atomPositions_.release();

    update();
}

void VizWidget::writeData()
{
    if (AtomItem::modified)
    {
        atomPositions_.bind();
        atomPositions_.write(0, AtomItem::getBuffer().constData(), AtomItem::getBuffer().size() * sizeof(VizBallInstance));
        atomPositions_.release();

        AtomItem::modified = false;
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

        sphereProgram_.setUniformValue("pro", projection_);
        sphereProgram_.setUniformValue("mv", modelView_);

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
