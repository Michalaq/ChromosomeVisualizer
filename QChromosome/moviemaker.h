#ifndef MOVIEMAKER_H
#define MOVIEMAKER_H

#include <fstream>

#include "../QtChromosomeViz_v2/VizWidget.hpp"
#include "camera.h"
#include "interpolator.h"

namespace MovieMaker
{
void makeMovie(QString filename, int frames, float framerate, int fps, QString suffix);
void captureScene(std::shared_ptr<Simulation> simulation, int gfn, const VizWidget* scene, const Camera* camera, QString suffix, const Interpolator& ip, int fn, int total);
void captureScene1(std::shared_ptr<Simulation> simulation, int fn, const VizWidget* scene, const Camera* camera, QString suffix);

void addSphere(std::ostream& outFile, const QVector3D & position, float radius, Material* color);
void addCylinder(std::ostream& outFile, const QVector3D & positionA, const QVector3D & positionB, float radius, Material* colorA, Material* colorB);
}

#endif // MOVIEMAKER_H
