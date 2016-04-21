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

    static void inline captureScene(const QVector<VizBallInstance> & vizBalls, const int connectionCount, const QVector3D & cameraLocation, const QVector3D & cameraLookAt, const RenderSettings & settings)
    {
        prepareINIFile(settings.getOutputWidth(), settings.getOutputHeight(), true);
        std::ofstream outFile;
        createPOVFile(outFile, settings.getFile().toStdString());
        setCamera(outFile, -cameraLocation.x(), cameraLocation.y(), cameraLocation.z(), -cameraLookAt.x(), cameraLookAt.y(), cameraLookAt.z(), settings.getOutputWidth(), settings.getOutputHeight());

        for (int i = 0; i < vizBalls.length(); i++)
            addSphere(outFile, -vizBalls[i].position.x(), vizBalls[i].position.y(), vizBalls[i].position.z(), vizBalls[i].size, vizBalls[i].color);

        for (int i = 0; i < connectionCount; i++)
        {
            addCylinder(outFile, -vizBalls[i].position.x(), vizBalls[i].position.y(), vizBalls[i].position.z(),
                        -vizBalls[i+1].position.x(), vizBalls[i+1].position.y(), vizBalls[i+1].position.z(), vizBalls[i].size / 3.f, vizBalls[i].color, vizBalls[i+1].color);
        }

//TODO: ponizej do ogarniecia
#ifdef __linux__
        qDebug() << "linux povray photo";
        system(QString("povray povray.ini Library_Path=/usr/local/share/povray-3.7/include/ %1.pov").arg(settings.getFile()).toUtf8().constData());
#elif _WIN32
        qDebug() << "windows povray photo";
        system((QString(R"~(""C:\Program Files\POV-Ray\v3.7\bin\pvengine64.exe"" povray.ini -D /RENDER )~") + QString::fromStdString(filename) + QString(".pov /EXIT")).toUtf8().constData());
#else
        qDebug() << "platform not supported";
#endif
    }

private:

    static void inline prepareINIFile(int width, int height, bool aa)
    {
        std::ofstream outFile("povray.ini");
        outFile << "Width=" << width << "\nHeight=" << height << "\nAntialias=" << (aa ? "On" : "Off") << std::endl;
    }

    static void inline createPOVFile(std::ofstream& outFile, std::string filename)
    {
        outFile.open(filename + ".pov");

        outFile << "#include \"colors.inc\"\n#include \"stones.inc\"" << std::endl;
    }

    static void inline setCamera(std::ofstream& outFile, float x, float y, float z, float lookx, float looky, float lookz, float reswidth, float resheight)
    {
        outFile << "camera{right x*" << reswidth << "/" << resheight << "\nlocation<" << x << ", " << y << ", " << z << ">look_at<" << lookx << ", " << looky << ", " << lookz << ">}\n" <<
            "light_source {<" << x << ", " << y << ", " << z << "> color White}"<< std::endl;
    }

    static void inline addSphere(std::ofstream& outFile, float x, float y, float z, float size, QColor color)
    {
        qreal r, g, b, t;
        color.getRgbF(&r, &g, &b, &t);

        outFile << "sphere{<" << x << ", " << y << ", " << z << ">, " << size << " texture{pigment{rgbt<" << r << ", " << g << ", " << b << ", " << 1. - t << ">}}}" << std::endl;
    }

    static void inline addCylinder(std::ofstream& outFile, float ax, float ay, float az, float bx, float by, float bz, float radius, QColor colora, QColor colorb)
    {
        qreal ar, ag, ab, at;
        colora.getRgbF(&ar, &ag, &ab, &at);

        qreal br, bg, bb, bt;
        colorb.getRgbF(&br, &bg, &bb, &bt);

        outFile << "cylinder{<" << ax << ", " << ay << ", " << az << ">, <" << bx << ", " << by << ", " << bz << ">," << radius <<
            " texture{pigment{gradient<" << ax - bx << ", " << ay - by << ", " << az - bz << "> color_map{[0.0 color rgb<" << ar << ", " << ag << ", " << ab << ">][1.0 color rgb<"
            << br << ", " << bg << ", " << bb << ">]}}}}" << std::endl;
    }
};

#endif // MOVIEMAKER_H
