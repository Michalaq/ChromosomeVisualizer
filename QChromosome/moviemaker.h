#ifndef MOVIEMAKER_H
#define MOVIEMAKER_H

#include <cstdlib>
#include <fstream>

#include "../QtChromosomeViz_v2/VizWidget.hpp"
#include "camera.h"
#include "rendersettings.h"


class MovieMaker
{
public:

    static void inline captureScene(const QVector<VizBallInstance> & vizBalls, const int connectionCount, const Camera & camera, const RenderSettings & settings)
    {
        prepareINIFile(settings.outputSize(), true);
        std::ofstream outFile;
        createPOVFile(outFile, settings.saveFile().toStdString());
        setCamera(outFile, camera.position(), camera.lookAt(), settings.outputSize());

        for (int i = 0; i < vizBalls.length(); i++)
            addSphere(outFile, vizBalls[i].position, vizBalls[i].size, vizBalls[i].color);

        for (int i = 0; i < connectionCount; i++)
            addCylinder(outFile, vizBalls[i].position, vizBalls[i + 1].position, vizBalls[i].size / 3, vizBalls[i].color, vizBalls[i + 1].color);

//TODO: ponizej do ogarniecia
#ifdef __linux__
        qDebug() << "linux povray photo";
        system(QString("povray povray.ini Library_Path=/usr/local/share/povray-3.7/include/ %1.pov").arg(settings.saveFile()).toUtf8().constData());
#elif _WIN32
        qDebug() << "windows povray photo";
        system((QString(R"~(""C:\Program Files\POV-Ray\v3.7\bin\pvengine64.exe"" povray.ini -D /RENDER )~") + QString::fromStdString(filename) + QString(".pov /EXIT")).toUtf8().constData());
#else
        qDebug() << "platform not supported";
#endif
    }

private:

    static void inline prepareINIFile(QSize size, bool aa)
    {
        std::ofstream outFile("povray.ini");
        outFile << "Width=" << size.width() << "\nHeight=" << size.height() << "\nAntialias=" << (aa ? "On" : "Off") << std::endl;
    }

    static void inline createPOVFile(std::ofstream& outFile, std::string filename)
    {
        outFile.open(filename + ".pov");

        outFile << "#include \"colors.inc\"\n#include \"stones.inc\"" << std::endl;
    }

    static void inline setCamera(std::ofstream& outFile, const QVector3D & position, const QVector3D & lookAt, QSize size)
    {
        outFile << "camera{right x*" << size.width() << "/" << size.height() << "\nlocation<" << -position.x() << ", " << position.y() << ", " << position.z() << ">look_at<" << -lookAt.x() << ", " << lookAt.y() << ", " << lookAt.z() << ">}\n" <<
            "light_source {<" << -position.x() << ", " << position.y() << ", " << position.z() << "> color White}"<< std::endl;
    }

    static void inline addSphere(std::ofstream& outFile, const QVector3D & position, float size, QColor color)
    {
        qreal r, g, b, t;
        color.getRgbF(&r, &g, &b, &t);

        outFile << "sphere{<" << -position.x() << ", " << position.y() << ", " << position.z() << ">, " << size << " texture{pigment{rgbt<" << r << ", " << g << ", " << b << ", " << 1. - t << ">}}}" << std::endl;
    }

    static void inline addCylinder(std::ofstream& outFile, const QVector3D & posA, const QVector3D & posB, float radius, QColor colorA, QColor colorB)
    {
        qreal ar, ag, ab, at;
        colorA.getRgbF(&ar, &ag, &ab, &at);

        qreal br, bg, bb, bt;
        colorB.getRgbF(&br, &bg, &bb, &bt);

        QVector3D length = posA - posB;

        outFile << "cylinder{<" << posA.x() << ", " << posA.y() << ", " << posA.z() << ">, <" << posB.x() << ", " << posB.y() << ", " << posB.z() << ">," << radius <<
            " texture{pigment{gradient<" << length.x() << ", " << length.y() << ", " << length.z() << "> color_map{[0.0 color rgb<" << ar << ", " << ag << ", " << ab << ">][1.0 color rgb<"
            << br << ", " << bg << ", " << bb << ">]}}}}" << std::endl;
    }
};

#endif // MOVIEMAKER_H
