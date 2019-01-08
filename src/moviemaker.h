#ifndef MOVIEMAKER_H
#define MOVIEMAKER_H

#include "session.h"
#include <QTemporaryDir>

class MovieMaker : public QObject
{
    Q_OBJECT
public:
    ~MovieMaker() override;

    static MovieMaker* getInstance();

    void captureScene(Session* session);

    static void addSphere(QTextStream& outFile, const QVector3D & position, float radius, int color);
    static void addCylinder(QTextStream& outFile, const QVector3D & positionA, const QVector3D & positionB, float radiusA, float radiusB, int colorA, int colorB);

    static void addSphere1(QTextStream& outFile, int id, float radius, int color);
    static void addCylinder1(QTextStream& outFile, int idA, int idB, float radiusA, float radiusB, int colorA, int colorB);

private:
    explicit MovieMaker(QObject* parent = 0);

    static MovieMaker* instance;

    QVector<QTemporaryDir*> history;

    QProcess p;

    QByteArray buffer;

    uint64_t cf, tf;

signals:
    void progressChanged(int);
};

QTextStream& operator<<(QTextStream& out, const QVector3D & vec);

QTextStream& operator<<(QTextStream& out, const QColor & col);

#endif // MOVIEMAKER_H
