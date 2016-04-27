#ifndef MOVIEMAKER_H
#define MOVIEMAKER_H

#include <cstdlib>
#include <fstream>

#include "camera.h"
#include "rendersettings.h"

#include <QSettings>
#include <QProcess>

std::ostream& operator<<(std::ostream& out, const QVector3D & vec)
{
    return out << "<" << -vec.x() << ", " << vec.y() << ", " << vec.z() << ">";
}

std::ostream& operator<<(std::ostream& out, const QColor & col)
{
    return out << "rgbt<" << col.redF() << ", " << col.greenF() << ", " << col.blueF() << ", " << 1. - col.alphaF() << ">";
}

class MovieMaker
{
public:

    static void inline captureScene(const QVector<VizBallInstance> & vizBalls, const int connectionCount, const Camera & camera, const RenderSettings & renderSettings)
    {
        prepareINIFile(renderSettings.outputSize(), true);
        std::ofstream outFile;
        createPOVFile(outFile, renderSettings.saveFile().toStdString());

        setCamera(outFile, camera, renderSettings.outputSize());

        for (int i = 0; i < vizBalls.length(); i++)
            addSphere(outFile, vizBalls[i].position, vizBalls[i].size, QColor::fromRgba(vizBalls[i].color));

        for (int i = 0; i < connectionCount; i++)
            addCylinder(outFile, vizBalls[i].position, vizBalls[i + 1].position, vizBalls[i].size / 3, QColor::fromRgba(vizBalls[i].color), QColor::fromRgba(vizBalls[i + 1].color));

        outFile.flush();

        QSettings settings;
//TODO: ponizej do ogarniecia
#ifdef __linux__
        QStringList argv;
        argv << "povray.ini" << "+L" + settings.value("povraypath", "/usr/local/share/povray-3.7").toString() + "/include/" << renderSettings.saveFile() + ".pov";
        QProcess::execute("povray", argv);
#elif _WIN32
        qDebug() << "windows povray photo";
        system((QString(R"~(""C:\Program Files\POV-Ray\v3.7\bin\pvengine64.exe"" povray.ini -D /RENDER )~") + settings.saveFile() + QString(".pov /EXIT")).toUtf8().constData());
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
        outFile << "#include \"colors.inc\"\n#include \"stones.inc\"\n"
                << "\n";
    }

    static void inline setCamera(std::ofstream& outFile, const Camera& camera, QSize size)
    {
        outFile << "camera{\n"
                << "right x*" << size.width() << "/" << size.height() << "\n"
                << "location " << camera.position() << "\n"
                << "look_at " << camera.lookAt() << "\n"
                << "angle " << camera.angle() << "\n"
                << "}\n"
                << "\n";

        outFile << "light_source {" << camera.position() << " " << "color " << QColor(Qt::white) << "}\n"
                << "\n";
    }

    static void inline addSphere(std::ofstream& outFile, const QVector3D & position, float size, QColor color)
    {
        outFile << "sphere{" << position << ", " << size << " texture{pigment{" << color << "}}}\n";
    }

    static void inline addCylinder(std::ofstream& outFile, const QVector3D & positionA, const QVector3D & positionB, float radius, QColor colorA, QColor colorB)
    {
        outFile << "cylinder{" << positionA << ", " << positionB << "," << radius <<
            " texture{pigment{gradient<" << positionA - positionB << "> color_map{[0.0 color " << colorA << "][1.0 color "
            << colorB << "]}}}}\n";
    }
};

#endif // MOVIEMAKER_H
