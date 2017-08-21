#ifndef MOVIEMAKER_H
#define MOVIEMAKER_H

#include <fstream>

#include "../QtChromosomeViz_v2/VizWidget.hpp"
#include "camera.h"
#include "interpolator.h"

namespace MovieMaker
{
void makeMovie(QString filename, int frames, float framerate, int fps, QString suffix);
void captureScene(int fbeg, int fend, const VizWidget* scene, const Camera* camera, const Interpolator& ip, QString suffix);
void captureScene1(int fn, const VizWidget* scene, const Camera* camera, QString suffix);

void addSphere(std::ostream& outFile, const QVector3D & position, float radius, const Material* color);
void addCylinder(std::ostream& outFile, const QVector3D & positionA, const QVector3D & positionB, float radius, const Material* colorA, const Material* colorB);
}

#endif // MOVIEMAKER_H
