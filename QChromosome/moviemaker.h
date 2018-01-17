#ifndef MOVIEMAKER_H
#define MOVIEMAKER_H

#include <fstream>

#include "../QtChromosomeViz_v2/bartekm_code/Simulation.h"
#include "viewport.h"
#include "camera.h"

namespace MovieMaker
{
void captureScene(int fbeg, int fend, const std::shared_ptr<Simulation> simulation, Viewport* viewport, const Camera* camera, QString suffix, int fr);
void captureScene1(int fn, const std::shared_ptr<Simulation> simulation, Viewport* viewport, const Camera* camera, QString suffix);

void addSphere(std::ostream& outFile, const QVector3D & position, float radius, const Material* color);
void addCylinder(std::ostream& outFile, const QVector3D & positionA, const QVector3D & positionB, float radiusA, float radiusB, const Material* colorA, const Material* colorB);

void addSphere1(std::ostream& outFile, int id, float radius, const Material* color);
void addCylinder1(std::ostream& outFile, int idA, int idB, float radiusA, float radiusB, const Material* colorA, const Material* colorB);
}

std::ostream& operator<<(std::ostream& out, const QVector3D & vec);

std::ostream& operator<<(std::ostream& out, const QColor & col);

#endif // MOVIEMAKER_H
