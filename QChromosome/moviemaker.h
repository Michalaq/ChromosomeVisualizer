#ifndef MOVIEMAKER_H
#define MOVIEMAKER_H

#include <cstdlib>
#include <fstream>

#include "camera.h"
#include "rendersettings.h"

std::ostream& operator<<(std::ostream& out, const QVector3D & vec)
{
    return out << -vec.x() << ", " << vec.y() << ", " << vec.z();
}

std::ostream& operator<<(std::ostream& out, const QColor & col)
{
    return out << col.redF() << ", " << col.greenF() << ", " << col.blueF() << ", " << 1. - col.alphaF();
}

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

        outFile.flush();

//TODO: ponizej do ogarniecia
#ifdef __linux__
        qDebug() << "linux povray photo";
        system(QString("povray povray.ini +L/usr/local/share/povray-3.7/include/ %1.pov").arg(settings.saveFile()).toUtf8().constData());
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
        outFile << "Width=" << size.width() << "\nHeight=" << size.height() << "\nAntialias=" << (aa ? "On" : "Off") << "\n";
    }

    static void inline createPOVFile(std::ofstream& outFile, std::string filename)
    {
        outFile.open(filename + ".pov");
        outFile << "#include \"colors.inc\"\n#include \"stones.inc\"\n";
    }

    static void inline setCamera(std::ofstream& outFile, const QVector3D & position, const QVector3D & lookAt, QSize size)
    {
        outFile << "camera{right x*" << size.width() << "/" << size.height() << "\nlocation<" << position << ">look_at<" << lookAt << ">}\n" <<
            "light_source {<" << position << "> color White}\n";
    }

    static void inline addSphere(std::ofstream& outFile, const QVector3D & position, float size, QColor color)
    {
        outFile << "sphere{<" << position << ">, " << size << " texture{pigment{rgbt<" << color << ">}}}\n";
    }

    static void inline addCylinder(std::ofstream& outFile, const QVector3D & positionA, const QVector3D & positionB, float radius, QColor colorA, QColor colorB)
    {
        outFile << "cylinder{<" << positionA << ">, <" << positionB << ">," << radius <<
            " texture{pigment{gradient<" << positionA - positionB << "> color_map{[0.0 color rgb<" << colorA << ">][1.0 color rgb<"
            << colorB << ">]}}}}\n";
    }
};

#endif // MOVIEMAKER_H
