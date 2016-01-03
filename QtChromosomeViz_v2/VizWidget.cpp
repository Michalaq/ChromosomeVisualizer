#include <cassert>
#include <cstddef>
#include "bartekm_code/PDBSimulation.h"
#include "VizWidget.hpp"

static const char * sphereVertexShaderCode =
"#version 330 core\n"
"layout(location = 0) in vec3 vVertexPosition;"
"layout(location = 1) in vec3 vVertexNormal;"
"layout(location = 2) in vec3 vInstancePosition;"
"layout(location = 3) in uint iInstanceFlags;"
"layout(location = 4) in uint iAtomID;"
"uniform mat4 mvp;"
"uniform mat3 mvNormal;"
"out vec4 vPosition;"
"out vec3 vNormal;"
"flat out uint iInstanceID;"
"flat out uint iFlags;"
"void main() {"
    "vec4 pos = mvp * vec4(vVertexPosition + vInstancePosition.xyz, 1);"
    "gl_Position = pos;"
    "vPosition = pos;"
	"vNormal = normalize(mvNormal * vVertexNormal);"
    "iInstanceID = iAtomID;"
    "iFlags = iInstanceFlags;"
"}"
;

static const char * cylinderVertexShaderCode =
"#version 330 core\n"
"layout(location = 0) in vec3 vVertexPosition;"
"layout(location = 1) in vec3 vVertexNormal;"
"layout(location = 2) in vec3 vConnectionStart;"
"layout(location = 3) in vec4 vRotationQuat;"
;

static const char * fragmentShaderCode =
"#version 330 core\n"
"uniform vec4 ucColorTable[4];"
"uniform vec2 uvScreenSize;"
"in vec4 vPosition;"
"in vec3 vNormal;"
"flat in uint iFlags;"
"out vec4 cColor;"
"void main() {"
    "vec2 vScreenPos = 0.5f * (vPosition.xy * uvScreenSize) / vPosition.w;"
    "float stripePhase = 0.5f * (vScreenPos.x + vScreenPos.y);"
    "float whitening = clamp(0.5f * (3.f * sin(stripePhase)), 0.f, 0.666f);"
    "vec4 baseColor = ucColorTable[iFlags & 3u];"
	"float lightness = (0.5 + 0.5 * 0.7 * (vNormal.x + vNormal.z));"
    "vec4 cDiffuse = vec4(baseColor.xyz * lightness, baseColor.a);"
    "float isSelected = ((iFlags & 4u) == 4u) ? 1.f : 0.f;"
    "cColor = mix(cDiffuse, vec4(1.f, 1.f, 1.f, 1.f), isSelected * whitening);"
	//"cColor = vec4(1, 1, 1, 1);"
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

static const char * planeVertexShaderCode =
"#version 330 core\n"
"uniform mat4 mvp;"
"void main() {"
	"float r = 100.0;"
	"float x = ((gl_VertexID & 1) == 1) ? r : -r;"
	"float z = ((gl_VertexID & 2) == 2) ? -r : r;"
	"gl_Position = mvp * vec4(x, 0, z, 1);"
"}"
;

static const char * planeFragmentShaderCode =
"#version 330 core\n"
"out vec4 cColor;"
"void main() {"
	"cColor = vec4(1, 1, 1, 1);"
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

static int atomTypeToInt(const std::string & s)
{
	if (s == "UNB")
		return 0;
	if (s == "BOU")
		return 1;
	if (s == "LAM")
		return 2;
	if (s == "BIN")
		return 3;

	assert(false);
	return -1;
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
	//rotation = QQuaternion::
}

// @bartekz: to będzie można usunąć jak zaczniesz używać drugiego konstruktora
static const char * SIMULATION_PATH =
        "D:\\kodziki\\bio\\MC_random_r10_avoid_last_b700.pdb";
        //"/home/bartek/Dokumenty/zpp/test.pdb";

VizWidget::VizWidget(QWidget *parent)
    : VizWidget(std::make_shared<PDBSimulation>(SIMULATION_PATH), parent)
{

}

VizWidget::VizWidget(std::shared_ptr<Simulation> simulation, QWidget *parent)
    : QOpenGLWidget(parent)
    , simulation(std::move(simulation))
    , needVBOUpdate(true)
    , isSelecting_(false)
    , pickingFramebuffer_(nullptr)
{
    QMatrix4x4 mv;
    mv.translate(0.f, -50.f, -100.f);
    mv.rotate(105.f, 0.f, 1.f, 0.f);
    mv.rotate(15.f, 0.f, 0.f, 1.f);
    setModelView(mv);
}

VizWidget::~VizWidget()
{
    delete pickingFramebuffer_;
}

void VizWidget::initializeGL()
{
	initializeOpenGLFunctions();

	glEnable(GL_DEPTH_TEST);

	assert(sphereModel.create());
	sphereModel.setUsagePattern(QOpenGLBuffer::StaticDraw);

	assert(atomPositions.create());
	atomPositions.setUsagePattern(QOpenGLBuffer::DynamicDraw);

	assert(vao.create());

	// Create sphere model
	QVector<VizVertex> sphereVerts = generateSphere(6, 8);
	sphereVertCount = sphereVerts.size();
	vao.bind();

	sphereModel.bind();
	sphereModel.allocate(sphereVerts.data(), sphereVerts.size() * sizeof(VizVertex));

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

	sphereModel.release();
	atomPositions.bind();

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

	glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);

	atomPositions.release();
	vao.release();

	planeVAO.create();

	// TODO: Getting the first frame should be somewhere else
	setFirstFrame();

	// Shaders
	assert(program.create());
	program.addShaderFromSourceCode(QOpenGLShader::Vertex, sphereVertexShaderCode);
	program.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderCode);
	assert(program.link());

	// Set atom colors
	static const QVector4D colors[] = {
		{ 1.f, 0.f, 0.f, 1.f },
		{ 0.f, 1.f, 0.f, 1.f },
		{ 0.f, 0.f, 1.f, 1.f },
		{ 1.f, 1.f, 1.f, 0.5f }
	};

	program.bind();
	program.setUniformValueArray("ucColorTable", colors, 4);
	program.release();

	assert(planeProgram.create());
	planeProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, planeVertexShaderCode);
	planeProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, planeFragmentShaderCode);
	assert(planeProgram.link());

    assert(pickingProgram.create());
    pickingProgram.addShaderFromSourceCode(QOpenGLShader::Vertex,
                                           sphereVertexShaderCode);
    pickingProgram.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                           pickingFragmentShaderCode);
    assert(pickingProgram.link());
}

void VizWidget::paintGL()
{
    //advanceFrame();
    if (needVBOUpdate)
    {
        generateSortedState();
        updateWholeFrameData();
        needVBOUpdate = false;
    }

	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);

	//advanceFrame();
	generateSortedState();
	updateWholeFrameData();

	QMatrix4x4 m = modelView;
	//m.rotate(1.f, 0.f, 1.f, 0.f);
	setModelView(m);

	painter.beginNativePainting();

	// Enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enable culling
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	vao.bind();
	planeProgram.bind();
	planeProgram.setUniformValue("mvp", modelViewProjection);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	planeProgram.release();
	vao.release();

	// If there are no spheres, my driver crashes
	if (sphereCount > 0)
	{
		vao.bind();
		program.bind();

		program.setUniformValue("mvp", modelViewProjection);
		program.setUniformValue("mvNormal", modelViewNormal);
        program.setUniformValue("uvScreenSize",
                                (float)size().width(),
                                (float)size().height());

		glDrawArraysInstanced(GL_TRIANGLES, 0, sphereVertCount, sphereCount);

		program.release();
		vao.release();
	}

	glDisable(GL_CULL_FACE);

	painter.endNativePainting();

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

void VizWidget::resizeGL(int w, int h)
{
	if (w == 0 || h == 0)
		return;

	const float ratio = (float)w / (float)h;

	projection.setToIdentity();
	projection.perspective(45.f, ratio, 0.1f, 1000.f);
	modelViewProjection = projection * modelView;
}

void VizWidget::setModelView(const QMatrix4x4 & mat)
{
	modelView = mat;
	modelViewNormal = mat.normalMatrix();
	modelViewProjection = projection * modelView;

    needVBOUpdate = true;
}

void VizWidget::setSimulation(std::shared_ptr<Simulation> dp)
{
	simulation = std::move(dp);
	setFirstFrame();
}

void VizWidget::advanceFrame()
{
    setFrame(frameNumber + 1);
}

void VizWidget::setFirstFrame()
{
    auto frame = simulation->getFrame(0);

    atomPositions.bind();
    atomPositions.allocate(frame->atoms.size() * sizeof(VizBallInstance));
    atomPositions.release();

    sphereCount = frame->atoms.size();
    selectedBitmap_.fill(false, frame->atoms.size());

    VizBallInstance dummy;
    frameState.fill(dummy, frame->atoms.size());

    setFrame(0);
}

void VizWidget::setFrame(frameNumber_t frame)
{
    frameNumber = frame;
    auto diff = simulation->getFrame(frameNumber);
    for (const auto & a : diff->atoms)
    {
        frameState[a.id - 1].position = QVector3D(a.x, a.y, a.z);
        frameState[a.id - 1].flags = atomTypeToInt(a.type);
        if (selectedBitmap_[a.id - 1])
            frameState[a.id - 1].flags |= SELECTED_FLAG;
        frameState[a.id - 1].atomID = a.id - 1;
    }

    needVBOUpdate = true;
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
	atomPositions.bind();
    auto * data = (VizBallInstance*)atomPositions.map(QOpenGLBuffer::WriteOnly);

    memcpy(data, sortedState.constData(), sortedState.size() * sizeof(VizBallInstance));
	
	atomPositions.unmap();
	atomPositions.release();
}

void VizWidget::mousePressEvent(QMouseEvent * event)
{
    isSelecting_ = true;
    selectionPoints_[0] = event->pos();
    selectionPoints_[1] = event->pos();
}

void VizWidget::mouseMoveEvent(QMouseEvent * event)
{
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

        selectedSpheres_ = pickSpheres();

        // New selection
        for (const auto & id : selectedSpheres_)
            selectedBitmap_[id] = !ctrl;

        // That's a hack, but it forces updating the flags
        setFrame(frameNumber);
        update();
    }
}

QList<unsigned int> VizWidget::selectedSpheres() const
{
    return selectedSpheres_;
}

void VizWidget::generateSortedState()
{
    auto sorter = [&](const VizBallInstance & a, const VizBallInstance & b) -> bool {
		float z1 = QVector4D::dotProduct(modelViewProjection.row(2),
            QVector4D(a.position, 1.f));
		float z2 = QVector4D::dotProduct(modelViewProjection.row(2),
            QVector4D(b.position, 1.f));
		return z1 > z2;
	};

	sortedState = frameState;
	qSort(sortedState.begin(), sortedState.end(), sorter); // Lol xD
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
        const auto realSize = pickingFramebuffer_->size().scaled(
                    PICKING_FRAMEBUFFER_DOWNSCALE,
                    PICKING_FRAMEBUFFER_DOWNSCALE,
                    Qt::IgnoreAspectRatio);

        if (pickingFramebuffer_->size().width() < downSize.width() ||
            pickingFramebuffer_->size().height() < downSize.height())
        {
            delete pickingFramebuffer_;
            pickingFramebuffer_ = nullptr;
        }
    }

    if (!pickingFramebuffer_)
    {
        pickingFramebuffer_ = new QOpenGLFramebufferObject(
                    downSize, QOpenGLFramebufferObject::Depth);
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

    if (sphereCount > 0)
    {
        vao.bind();
        pickingProgram.bind();

        pickingProgram.setUniformValue("mvp", modelViewProjection);
        pickingProgram.setUniformValue("mvNormal", modelViewNormal);

        glDrawArraysInstanced(GL_TRIANGLES, 0, sphereVertCount, sphereCount);

        pickingProgram.release();
        vao.release();
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
