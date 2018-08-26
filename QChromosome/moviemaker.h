#ifndef MOVIEMAKER_H
#define MOVIEMAKER_H

#include <QThread>
#include "session.h"

class MovieMaker : public QThread
{
    Q_OBJECT
public:
    static MovieMaker* getInstance();

    void captureScene(int fbeg, int fend, Session* session, QString suffix);
    void captureScene1(Session* session, QString suffix);

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
    int fbeg_, fend_;
    QString suffix_;
    Session* session_;

    void captureScene_(int fbeg, int fend, QString suffix, Session* session);
    void captureScene1_(QString suffix, Session* session);

signals:
    void progressChanged(int);
};

QTextStream& operator<<(QTextStream& out, const QVector3D & vec);

QTextStream& operator<<(QTextStream& out, const QColor & col);

#endif // MOVIEMAKER_H
