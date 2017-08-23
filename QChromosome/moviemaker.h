#ifndef MOVIEMAKER_H
#define MOVIEMAKER_H

#include <fstream>

#include "../QtChromosomeViz_v2/VizWidget.hpp"
#include "camera.h"
#include "interpolator.h"

namespace MovieMaker
{
void captureScene(int fbeg, int fend, const VizWidget* scene, const Camera* camera, const Interpolator& ip, QString suffix, int fr);
void captureScene1(int fn, const VizWidget* scene, const Camera* camera, QString suffix);

void addSphere(std::ostream& outFile, const QVector3D & position, float radius, const Material* color);
void addCylinder(std::ostream& outFile, const QVector3D & positionA, const QVector3D & positionB, float radiusA, float radiusB, const Material* colorA, const Material* colorB);

void addSphere1(std::ostream& outFile, int id, float radius, const Material* color);
void addCylinder1(std::ostream& outFile, int idA, int idB, float radiusA, float radiusB, const Material* colorA, const Material* colorB);
}

#endif // MOVIEMAKER_H
