#ifndef MOVIEMAKER_H
#define MOVIEMAKER_H

#include "../QtChromosomeViz_v2/bartekm_code/Simulation.h"
#include "viewport.h"
#include "camera.h"

class MovieMaker : public QObject
{
    Q_OBJECT
public:
    static const MovieMaker* getInstance();

    void captureScene(int fbeg, int fend, const std::shared_ptr<Simulation> simulation, Viewport* viewport, const Camera* camera, QString suffix, int fr) const;
    void captureScene1(int fn, const std::shared_ptr<Simulation> simulation, Viewport* viewport, const Camera* camera, QString suffix) const;

    void addSphere(QTextStream& outFile, const QVector3D & position, float radius, const Material* color) const;
    void addCylinder(QTextStream& outFile, const QVector3D & positionA, const QVector3D & positionB, float radiusA, float radiusB, const Material* colorA, const Material* colorB) const;

    void addSphere1(QTextStream& outFile, int id, float radius, const Material* color) const;
    void addCylinder1(QTextStream& outFile, int idA, int idB, float radiusA, float radiusB, const Material* colorA, const Material* colorB) const;

private:
    explicit MovieMaker(QObject* parent = 0);

    static MovieMaker* instance;
};

QTextStream& operator<<(QTextStream& out, const QVector3D & vec);

QTextStream& operator<<(QTextStream& out, const QColor & col);

#endif // MOVIEMAKER_H
