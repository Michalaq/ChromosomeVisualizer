#include <cassert>
#include "bartekm_code/PDBSimulation.h"
#include "VizWidget.hpp"

static const char * sphereVertexShaderCode =
"#version 330 core\n"
"layout(location = 0) in vec3 vVertexPosition;"
"layout(location = 1) in vec3 vVertexNormal;"
"layout(location = 2) in vec4 vInstancePosition;"
"uniform mat4 mvp;"
"uniform mat3 mvNormal;"
"out vec3 vNormal;"
"flat out int iType;"
"void main() {"
	"gl_Position = mvp * vec4(vVertexPosition + vInstancePosition.xyz, 1);"
	"vNormal = normalize(mvNormal * vVertexNormal);"
	"iType = int(vInstancePosition.w);"
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
"in vec3 vNormal;"
"flat in int iType;"
"out vec4 cColor;"
"void main() {"
//"float alpha = 0.25 + float(iType) / 4;"
	"vec4 baseColor = ucColorTable[iType];"
	"float lightness = (0.5 + 0.5 * 0.7 * (vNormal.x + vNormal.z));"
	"cColor = vec4(baseColor.xyz * lightness, baseColor.a);"
	//"cColor = vec4(1, 1, 1, 1);"
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
        //"D:\\kodziki\\bio\\MC_random_r10_avoid_last_b700.pdb";
        "/home/bart/Pobrane/MC_random_r10_avoid_last_b700.pdb";

VizWidget::VizWidget(QWidget *parent)
    : VizWidget(std::make_shared<PDBSimulation>(SIMULATION_PATH), parent)
{

}

VizWidget::VizWidget(std::shared_ptr<Simulation> simulation, QWidget *parent)
    : QOpenGLWidget(parent)
    , simulation(std::move(simulation))
{
    QMatrix4x4 mv;
    mv.translate(0.f, -50.f, -100.f);
    mv.rotate(105.f, 0.f, 1.f, 0.f);
    mv.rotate(15.f, 0.f, 0.f, 1.f);
    setModelView(mv);
}

VizWidget::~VizWidget()
{
	
}

void VizWidget::initializeGL()
{
	initializeOpenGLFunctions();

	glEnable(GL_DEPTH_TEST);

	// Enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enable culling
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

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
		4,
		GL_FLOAT,
		GL_FALSE,
		sizeof(QVector4D),
		nullptr
	);

	glVertexAttribDivisor(2, 1);

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
}

void VizWidget::paintGL()
{
    //advanceFrame();
	generateSortedState();
	updateWholeFrameData();

	QMatrix4x4 m = modelView;
    //m.rotate(1.f, 0.f, 1.f, 0.f);
	setModelView(m);

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

		glDrawArraysInstanced(GL_TRIANGLES, 0, sphereVertCount, sphereCount);

		program.release();
		vao.release();
	}
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
}

void VizWidget::setSimulation(std::shared_ptr<Simulation> dp)
{
	simulation = std::move(dp);
	setFirstFrame();
}

void VizWidget::advanceFrame()
{
	/*atomPositions.bind();

	QVector3D * data = (QVector3D*)atomPositions.map(QOpenGLBuffer::WriteOnly);
	auto diff = diffProvider->getNextDiff();
	for (const auto & a : diff->atoms)
		data[a.id - 1] = QVector3D(a.x, a.y, a.z);
	atomPositions.unmap();

	atomPositions.release();*/

	frameNumber++;
	auto diff = simulation->getFrame(frameNumber);
	for (const auto & a : diff->atoms)
		frameState[a.id - 1] = QVector4D(a.x, a.y, a.z, (float)atomTypeToInt(a.type));
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

void VizWidget::setFirstFrame()
{
	frameNumber = 0;
	auto frame = simulation->getFrame(frameNumber);

	QVector<QVector4D> instances;
	for (const auto & a : frame->atoms)
		instances.push_back(QVector4D(a.x, a.y, a.z, (float)atomTypeToInt(a.type)));

	sphereCount = instances.size();

	atomPositions.bind();
	atomPositions.allocate(instances.data(), instances.size() * sizeof(QVector4D));
	atomPositions.release();

	frameState = std::move(instances);
}

void VizWidget::updateWholeFrameData()
{
	atomPositions.bind();
	QVector4D * data = (QVector4D*)atomPositions.map(QOpenGLBuffer::WriteOnly);

	memcpy(data, sortedState.constData(), sortedState.size() * sizeof(QVector4D));
	
	atomPositions.unmap();
	atomPositions.release();
}

void VizWidget::generateSortedState()
{
	auto sorter = [&](const QVector4D & a, const QVector4D & b) -> bool {
		float z1 = QVector4D::dotProduct(modelViewProjection.row(2),
			QVector4D(a.toVector3D(), 1.f));
		float z2 = QVector4D::dotProduct(modelViewProjection.row(2),
			QVector4D(b.toVector3D(), 1.f));
		return z1 > z2;
	};

	sortedState = frameState;
	qSort(sortedState.begin(), sortedState.end(), sorter); // Lol xD
}
