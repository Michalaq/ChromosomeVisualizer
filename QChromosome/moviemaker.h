#ifndef MOVIEMAKER_H
#define MOVIEMAKER_H

#include <fstream>

#include <QVector3D>

#include "../QtChromosomeViz_v2/VizWidget.hpp"
#include "camera.h"
#include "rendersettings.h"

#include <QSettings>
#include <QProcess>
#include "interpolator.h"

std::ostream& operator<<(std::ostream& out, const QVector3D & vec)
{
    return out << "<" << -vec.x() << ", " << vec.y() << ", " << vec.z() << ">";
}

std::ostream& operator<<(std::ostream& out, const QColor & col)
{
    return out << "rgbt<" << col.redF() << ", " << col.greenF() << ", " << col.blueF() << ", " << 1. - col.alphaF() * col.alphaF() << ">";
}

class MovieMaker
{
public:

    static void inline makeMovie(QString filename, int frames, float framerate, int fps, QString suffix)
    {
        QStringList argv;
        argv << "-framerate " + QString::number(framerate) << "-i " + filename + "%0" + QString::number(QString::number(frames).length()) + ".png" << "-c:v libx264"
             << "-r " + QString::number(fps) << "-pix_fmt yuv420p" << filename + ".mp4";
        //QProcess::execute("ffmpeg", argv);
        QProcess::execute(QString("ffmpeg ") + "-y" + " -framerate " + QString::number(framerate) + " -i " + filename + "%0" + QString::number(QString::number(frames).length()) + "d.png" + " -c:v libx264"
                          + " -r " + QString::number(fps) + " -pix_fmt yuv420p file:" + filename + suffix + ".mp4");
    }

    static void inline captureScene(const VizWidget* scene, const Camera* camera, QString suffix, const Interpolator& ip, int fn)
    {
        auto renderSettings = RenderSettings::getInstance();
        prepareINIFile(renderSettings);
        std::ofstream outFile;
        QString filename = renderSettings->saveFile() + suffix;
        createPOVFile(outFile, filename.toStdString(), renderSettings);

        if (!ip.keys().isEmpty())
            outFile << ip;

        if (renderSettings->cam360())
            set360Camera(outFile, camera, renderSettings->outputSize());
        else
            setCamera1(outFile, camera, renderSettings->outputSize(), fn, ip.keys().isEmpty());
        setBackgroundColor(outFile, scene->backgroundColor());
        setFog(outFile, scene->backgroundColor(), 1.f / scene->fogDensity()); //TODO: dobre rownanie dla ostatniego argumentu

        auto& vizBalls = scene->getBallInstances();

        for (auto b : vizBalls)
            addSphere(outFile, b.position, b.size, QColor::fromRgba(b.color));

        auto frame = scene->currentFrame();

        for (auto c : frame->connectedRanges)
        {
            for (int i = c.first; i < c.second; i++)
                addCylinder(outFile, vizBalls[i - 1].position, vizBalls[i].position, vizBalls[i - 1].size / 2, QColor::fromRgba(vizBalls[i - 1].color), QColor::fromRgba(vizBalls[i].color));
        }

        for (auto & key : scene->getLabels().keys())
            addLabel(outFile, "");

        outFile.flush();

        QSettings settings;

#ifdef __linux__
        QStringList argv;
        argv << "povray.ini" << "-D" << "+L" + settings.value("povraypath", "/usr/local/share/povray-3.7").toString() + "/include/" << filename + ".pov";
        QProcess::execute("povray", argv);
#elif _WIN32
        qDebug() << "windows povray photo";
        system((QString(R"~(""C:\Program Files\POV-Ray\v3.7\bin\pvengine64.exe"" povray.ini -D /RENDER )~") + settings.saveFile() + QString(".pov /EXIT")).toUtf8().constData());
#else
        qDebug() << "platform not supported";
#endif
    }

    static void inline captureScene1(const VizWidget* scene, const Camera* camera, QString suffix)
    {
        auto renderSettings = RenderSettings::getInstance();
        prepareINIFile(renderSettings);
        std::ofstream outFile;
        QString filename = renderSettings->saveFile() + suffix;
        createPOVFile(outFile, filename.toStdString(), renderSettings);

        if (renderSettings->cam360())
            set360Camera(outFile, camera, renderSettings->outputSize());
        else
            setCamera(outFile, camera, renderSettings->outputSize());
        setBackgroundColor(outFile, scene->backgroundColor());
        setFog(outFile, scene->backgroundColor(), 1.f / scene->fogDensity()); //TODO: dobre rownanie dla ostatniego argumentu

        auto& vizBalls = scene->getBallInstances();

        for (auto b : vizBalls)
            addSphere(outFile, b.position, b.size, QColor::fromRgba(b.color));

        auto frame = scene->currentFrame();

        for (auto c : frame->connectedRanges)
        {
            for (int i = c.first; i < c.second; i++)
                addCylinder(outFile, vizBalls[i - 1].position, vizBalls[i].position, vizBalls[i - 1].size / 2, QColor::fromRgba(vizBalls[i - 1].color), QColor::fromRgba(vizBalls[i].color));
        }

        for (auto & key : scene->getLabels().keys())
            addLabel(outFile, "");

        outFile.flush();

        QSettings settings;

#ifdef __linux__
        QStringList argv;
        argv << "povray.ini" << "-D" << "+V" << "+L" + settings.value("povraypath", "/usr/local/share/povray-3.7").toString() + "/include/" << filename + ".pov";

        QProcess *p = new QProcess;
        p->setCurrentReadChannel(QProcess::StandardError);
        p->start("povray", argv);

        QObject::connect(p, &QProcess::readyRead, [=]() {
           ;
        });

        QObject::connect(p, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), [=](int exitCode, QProcess::ExitStatus exitStatus){
            system(QString(QString("rm ") + filename + ".pov").toUtf8().constData());
            system("rm povray.ini");

            if (renderSettings->openFile() && exitStatus == QProcess::NormalExit && exitCode == 0)
                system(QString(QString("xdg-open ") + filename + ".png").toUtf8().constData());

            p->deleteLater();
        });
#elif _WIN32
        qDebug() << "windows povray photo";
        system((QString(R"~(""C:\Program Files\POV-Ray\v3.7\bin\pvengine64.exe"" povray.ini -D /RENDER )~") + settings.saveFile() + QString(".pov /EXIT")).toUtf8().constData());
#else
        qDebug() << "platform not supported";
#endif
    }

private:

    static void inline prepareINIFile(const RenderSettings * renderSettings)
    {
        std::ofstream outFile("povray.ini");
        QSize size = renderSettings->outputSize();
        outFile << "Width=" << size.width() << "\nHeight=" << size.height()
                << "\nQuality=" << renderSettings->quality().toStdString();
        if (renderSettings->antiAliasing())
        {
            outFile << "\nAntialias=on"
                    << "\nSampling_Method=" << renderSettings->aaSamplingMethod().toStdString()
                    << "\nAntialias_Threshold=" << renderSettings->aaThreshold().toStdString()
                    << "\nAntialias_Depth=" << renderSettings->aaDepth().toStdString();
            if (renderSettings->aaJitter())
            {
                outFile << "\nJitter=on"
                        << "\nJitter_Amount=" << renderSettings->aaJitterAmount().toStdString();
            }
            else
            {
                outFile << "\nJitter=off";
            }
        }
        else
        {
            outFile << "\nAntialias=off";
        }
    }

    static void inline createPOVFile(std::ofstream& outFile, std::string filename, const RenderSettings * renderSettings)
    {
        outFile.open(filename + ".pov");
        outFile << "#version 3.7\n"
                << "global_settings { assumed_gamma 1.0 }\n";
        outFile << "#default{finish{ambient " << renderSettings->ambient().toStdString()
                << " diffuse " << renderSettings->diffuse().toStdString()
                << " phong " << renderSettings->phong().toStdString()
                << " phong_size " << renderSettings->phongSize().toStdString()
                << " metallic " << renderSettings->metallic().toStdString()
                << " irid { " << renderSettings->iridescence().toStdString()
                << " thickness " << renderSettings->iridescenceThickness().toStdString()
                << " turbulence " << renderSettings->iridescenceTurbulence().toStdString()
                << "} specular 1.0"
                << " roughness 0.02"
                << "}}\n"
                << "#include \"colors.inc\"\n#include \"stones.inc\"\n";
    }

    static void inline setCamera(std::ofstream& outFile, const Camera* camera, QSize size)
    {
        outFile << "camera{perspective\n"
                << "right x*" << size.width() << "/" << size.height() << "\n"
                << "look_at -z\n"
                << "angle " << camera->angle() << "\n"
                << "rotate " << -camera->EulerAngles() << "\n"
                << "translate " << camera->position() << "\n"
                << "}\n"
                << "\n";

        outFile << "light_source {\n"
                << QVector3D() << "," << QColor(Qt::white) << "\n"
                << "parallel\n"
                << "point_at " << -QVector3D(1., 1., 2.) << "\n"
                << "}\n";
    }

    static void inline setCamera1(std::ofstream& outFile, const Camera* camera, QSize size, double fn, bool s)
    {
        outFile << "camera{perspective\n"
                << "right x*" << size.width() << "/" << size.height() << "\n"
                << "look_at -z\n"
                << "angle " << camera->angle() << "\n";

        if (s)
            outFile << "rotate " << -camera->EulerAngles() << "\n"
                << "translate " << camera->position() << "\n";
        else
            outFile << "rotate -MySplineAng(" << fn << ")\n"
                << "translate MySplinePos(" << fn << ")\n";

        outFile << "}\n"
                << "\n";

        outFile << "light_source {\n"
                << QVector3D() << "," << QColor(Qt::white) << "\n"
                << "parallel\n"
                << "point_at " << -QVector3D(1., 1., 2.) << "\n"
                << "}\n";
    }

    static void inline set360Camera(std::ofstream& outFile, const Camera* camera, QSize size)
    {
        outFile << "camera{spherical \n"
                << "right x*" << size.width() << "/" << size.height() << "\n"
                << "location " << camera->position() << "\n"
                << "look_at " << camera->lookAt() << "\n"
                << "}\n"
                << "\n";

        outFile << "light_source {" << camera->position() << " " << "color " << QColor(Qt::white) << "}\n"
                << "\n";
    }

    static void inline setBackgroundColor(std::ofstream& outFile, const QColor & color)
    {
        outFile << "background{color " << color << "}\n";
    }

    static void inline setFog(std::ofstream& outFile, const QColor & color, const float distance)
    {
        outFile << "fog{color " << color << " distance " << distance << " }\n";
    }

    static void inline addSphere(std::ofstream& outFile, const QVector3D & position, float size, QColor color)
    {
        outFile << "sphere{" << position << ", " << size << " texture{pigment{" << color << "}}}\n";
    }

    static void inline addCylinder(std::ofstream& outFile, const QVector3D & positionA, const QVector3D & positionB, float radius, QColor colorA, QColor colorB)
    {
        QVector3D direction = positionB - positionA;
        qreal dist = qFabs(QVector3D::dotProduct(positionA, direction)) / direction.length(); //distance of positionA from plane defined by normal vector 'direction' and point (0,0,0)
        outFile << "cylinder{" << positionA << ", " << positionB << ", " << radius <<
            " texture{pigment{gradient" << direction << " color_map{[0.0 color " << colorA << "][1.0 color " <<
            colorB << "]} phase " << (dist - static_cast<int>(dist / direction.length()) * direction.length()) / direction.length() <<
            " scale " << direction.length() << "}}}\n";
    }

    static void inline addLabel(std::ofstream& outFile, const QString & label)
    {

    }
};

#endif // MOVIEMAKER_H
