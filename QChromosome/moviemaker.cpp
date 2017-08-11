#include "moviemaker.h"

#include "rendersettings.h"

#include <QSettings>
#include <QProcess>

std::ostream& operator<<(std::ostream& out, const QVector3D & vec)
{
    return out << "<" << -vec.x() << ", " << vec.y() << ", " << vec.z() << ">";
}

std::ostream& operator<<(std::ostream& out, const QColor & col)
{
    return out << "rgbt<" << col.redF() << ", " << col.greenF() << ", " << col.blueF() << ", " << 1. - col.alphaF() * col.alphaF() << ">";
}

void prepareINIFile(const RenderSettings * renderSettings)
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

void createPOVFile(std::ofstream& outFile, std::string filename, const RenderSettings * renderSettings)
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

void setCamera(std::ofstream& outFile, const Camera* camera, QSize size)
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

void setCamera1(std::ofstream& outFile, const Camera* camera, QSize size, double fn, bool s)
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

void set360Camera(std::ofstream& outFile, const Camera* camera, QSize size)
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

void setBackgroundColor(std::ofstream& outFile, const QColor & color)
{
    outFile << "background{color " << color << "}\n";
}

void setFog(std::ofstream& outFile, const QColor & color, const float distance)
{
    outFile << "fog{color " << color << " distance " << distance << " }\n";
}

void addLabel(std::ofstream& outFile, const QString & label)
{

}

void MovieMaker::makeMovie(QString filename, int frames, float framerate, int fps, QString suffix)
{
    QStringList argv;
    argv << "-framerate " + QString::number(framerate) << "-i " + filename + "%0" + QString::number(QString::number(frames).length()) + ".png" << "-c:v libx264"
         << "-r " + QString::number(fps) << "-pix_fmt yuv420p" << filename + ".mp4";
    //QProcess::execute("ffmpeg", argv);
    QProcess::execute(QString("ffmpeg ") + "-y" + " -framerate " + QString::number(framerate) + " -i " + filename + "%0" + QString::number(QString::number(frames).length()) + "d.png" + " -c:v libx264"
                      + " -r " + QString::number(fps) + " -pix_fmt yuv420p file:" + filename + suffix + ".mp4");
}

void MovieMaker::captureScene(int gfn, const VizWidget* scene, const Camera* camera, QString suffix, const Interpolator& ip, int fn, int total)
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
        setCamera1(outFile, camera, renderSettings->outputSize(), gfn, ip.keys().isEmpty());
    setBackgroundColor(outFile, scene->backgroundColor());
    setFog(outFile, scene->backgroundColor(), 1.f / scene->fogDensity()); //TODO: dobre rownanie dla ostatniego argumentu

    scene->writePOVFrame(outFile, fn);

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

    QImage img;
    img.load(filename + ".png");
    QPainter p(&img);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::white);
    QFont f;
    f.setFamily("RobotoMono");
    f.setPixelSize(img.height()/20);
    p.setFont(f);
    int w = img.width()-10;
    int h = img.height()/20+20;
    p.drawText(QRect(0, 0, w, h), QString("Frame %1/%2").arg(QString::number(fn).rightJustified(QString::number(total).length(), '0')).arg(total), Qt::AlignRight | Qt::AlignVCenter);
    p.drawText(QRect(0, h/2, w, h), QString("Time %1").arg(QString::number(gfn).rightJustified(QString::number(gfn-fn+total).length(), '0')), Qt::AlignRight | Qt::AlignVCenter);
    img.save(filename + ".png", "PNG");
}

void MovieMaker::captureScene1(int fn, const VizWidget* scene, const Camera* camera, QString suffix)
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

    scene->writePOVFrame(outFile, fn);

    for (auto & key : scene->getLabels().keys())
        addLabel(outFile, "");

    outFile.flush();

    QSettings settings;

#ifdef __linux__
    QStringList argv;
    argv << "povray.ini" << "-D" << "+V" << "+L" + settings.value("povraypath", "/usr/local/share/povray-3.7").toString() + "/include/" << filename + ".pov";

    QProcess *p = new QProcess;
    //p->setCurrentReadChannel(QProcess::StandardError);
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

void MovieMaker::addSphere(std::ostream& outFile, const QVector3D & position, float radius, const Material *color)
{
    outFile << "sphere {"
            << position << ", "
            << radius << " "
            << "texture { m" << QString::number(quintptr(color), 16).toStdString() << " }"
            << "}\n";
}

void MovieMaker::addCylinder(std::ostream& outFile, const QVector3D & positionA, const QVector3D & positionB, float radius, const Material *colorA, const Material *colorB)
{
    QVector3D direction = positionB - positionA;

    outFile << "cylinder{" << positionA << ", " << positionB << ", " << radius
            << " texture{gradient" << direction.normalized() << " texture_map{[0.0 m" << QString::number(quintptr(colorA), 16).toStdString() << "][1.0 m" << QString::number(quintptr(colorB), 16).toStdString() << "]}"
            << " scale " << direction.length()
            << " translate " << positionA
            << "}}\n";
}
