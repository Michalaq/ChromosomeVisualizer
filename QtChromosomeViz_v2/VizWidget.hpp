#ifndef VIZWINDOW_HPP
#define VIZWINDOW_HPP

#include <memory>

#include <QtGui>
#include <QtOpenGL>
#include <QOpenGLFunctions_3_3_Core>

#include "bartekm_code/Simulation.h"

struct VizVertex
{
public:
	QVector3D position;
	QVector3D normal;

	VizVertex rotated(const QQuaternion & q) const;
};

struct VizBallInstance
{
public:
    QVector3D position;
    unsigned int flags;
    unsigned int atomID;
};

struct VizLink
{
public:
	QVector3D position;
	QQuaternion rotation;

	void update(const QVector3D & p1, const QVector3D & p2);
};

using VizSegment = QPair<VizVertex, VizVertex>;

class VizWidget :	public QOpenGLWidget,
					protected QOpenGLFunctions_3_3_Core
{
	Q_OBJECT;

public:
    VizWidget(QWidget *parent = 0);
    VizWidget(std::shared_ptr<Simulation> simulation, QWidget *parent = 0);
	virtual ~VizWidget();

	virtual void initializeGL() override;
    virtual void paintGL() override;
	virtual void resizeGL(int w, int h) override;

	void setModelView(const QMatrix4x4 & mat);
	void setSimulation(std::shared_ptr<Simulation> dp);

	void advanceFrame();
    void setFrame(frameNumber_t frame);

signals:
    void selectionChanged(const QList<unsigned int> & selected,
                          const QList<unsigned int> & deselected);

protected:
    // Generates vertices for a solid of revolution based on the given outline.
	//   quads - line segments disjoint from the axis of rotation
	//   axis - axis of rotation. Must not be zero.
	//   segments - number of segments to be generated.
	static QVector<VizVertex> generateSolidOfRevolution(
		const QVector<VizSegment> & quads,
		QVector3D axis,
		unsigned int segments
	);

	static QVector<VizVertex> generateSphere(unsigned int rings, unsigned int segments);
	static QVector<VizVertex> generateIcoSphere(unsigned int subdivisions);
	static QVector<VizVertex> generateCylinder(unsigned int segments);

	void setFirstFrame();
	void updateWholeFrameData();

    virtual void mousePressEvent(QMouseEvent * event) override final;
    virtual void mouseMoveEvent(QMouseEvent * event) override final;
    virtual void mouseReleaseEvent(QMouseEvent * event) override final;

    QList<unsigned int> selectedSpheres() const;

private:
	QOpenGLBuffer sphereModel;
	QOpenGLBuffer atomPositions;
	QOpenGLVertexArrayObject vao;

	QOpenGLVertexArrayObject planeVAO;

	QOpenGLShaderProgram program;
	QOpenGLShaderProgram planeProgram;
    QOpenGLShaderProgram pickingProgram;

	QMatrix4x4 projection;
	QMatrix4x4 modelViewProjection;
	QMatrix4x4 modelView;
	QMatrix3x3 modelViewNormal;

	unsigned int sphereVertCount;
	unsigned int sphereCount;

	std::shared_ptr<Simulation> simulation;
	frameNumber_t frameNumber;
	
    bool needVBOUpdate;
    QVector<VizBallInstance> frameState, sortedState;
    void generateSortedState();

    bool isSelecting_;
    QPoint selectionPoints_[2];
//    std::unique_ptr<QOpenGLFramebufferObject> pickingFramebuffer_;
    QOpenGLFramebufferObject * pickingFramebuffer_;
    QList<unsigned int> selectedSpheres_;
    QVector<bool> selectedBitmap_;

    QRect selectionRect() const;
    QList<unsigned int> pickSpheres();
};

#endif /* VIZWINDOW_HPP */
