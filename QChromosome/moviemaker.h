#ifndef MOVIEMAKER_H
#define MOVIEMAKER_H

#include <QThread>

#include "../QtChromosomeViz_v2/bartekm_code/Simulation.h"
#include "camera.h"

class MovieMaker : public QThread
{
    Q_OBJECT
public:
    static MovieMaker* getInstance();

    void captureScene(int fbeg, int fend, const std::shared_ptr<Simulation> simulation, const Camera* camera, QString suffix, int fr);
    void captureScene1(int fn, const std::shared_ptr<Simulation> simulation, const Camera* camera, QString suffix);

    static void addSphere(QTextStream& outFile, const QVector3D & position, float radius, int color);
    static void addCylinder(QTextStream& outFile, const QVector3D & positionA, const QVector3D & positionB, float radiusA, float radiusB, int colorA, int colorB);

    static void addSphere1(QTextStream& outFile, int id, float radius, int color);
    static void addCylinder1(QTextStream& outFile, int idA, int idB, float radiusA, float radiusB, int colorA, int colorB);

protected:
    void run() override;

private:
    explicit MovieMaker(QObject* parent = 0);

    static MovieMaker* instance;

    bool snapshot;
    int fbeg_, fend_, fr_, fn_;
    std::shared_ptr<Simulation> simulation_;
    const Camera* camera_;
    QString suffix_;

    void captureScene_(int fbeg, int fend, const std::shared_ptr<Simulation> simulation, const Camera* camera, QString suffix, int fr);
    void captureScene1_(int fn, const std::shared_ptr<Simulation> simulation, const Camera* camera, QString suffix);

signals:
    void progressChanged(int);
};

QTextStream& operator<<(QTextStream& out, const QVector3D & vec);

QTextStream& operator<<(QTextStream& out, const QColor & col);

#endif // MOVIEMAKER_H
